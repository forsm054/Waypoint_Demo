/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Node.hh"

#include "Alias.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExecConnector.hh"
#include "ExpressionConstants.hh"
#include "ExternalInterface.hh"
#include "NodeConstants.hh"
#include "PlexilExec.hh"
#include "SimpleMap.hh"
#include "UserVariable.hh"
#include "lifecycle-utils.h"
#include "map-utils.hh"

#include <algorithm> // for std::sort
#include <cfloat>    // for DBL_MAX
#include <iomanip>   // for std::setprecision
#include <sstream>

namespace PLEXIL {

  // Initialize static variable
  static std::vector<std::string> s_allConditions;

  static void initAllConditions()
  {
    check_error_1(s_allConditions.empty()); // cheap sanity check
    s_allConditions.reserve(Node::conditionIndexMax);

    // *** N.B.: Order MUST agree with enum ConditionIndex!
    // Conditions on parent
    s_allConditions.push_back(Node::ANCESTOR_EXIT_CONDITION());
    s_allConditions.push_back(Node::ANCESTOR_INVARIANT_CONDITION());
    s_allConditions.push_back(Node::ANCESTOR_END_CONDITION());
    // User specified conditions
    s_allConditions.push_back(Node::SKIP_CONDITION());
    s_allConditions.push_back(Node::START_CONDITION());
    s_allConditions.push_back(Node::PRE_CONDITION());
    s_allConditions.push_back(Node::EXIT_CONDITION());
    s_allConditions.push_back(Node::INVARIANT_CONDITION());
    s_allConditions.push_back(Node::END_CONDITION());
    s_allConditions.push_back(Node::POST_CONDITION());
    s_allConditions.push_back(Node::REPEAT_CONDITION());
    // For all but Empty nodes
    s_allConditions.push_back(Node::ACTION_COMPLETE());
    // For all but Empty and Update nodes
    s_allConditions.push_back(Node::ABORT_COMPLETE());

    // inexpensive sanity check
    assertTrue_2(s_allConditions.size() == Node::conditionIndexMax,
                 "INTERNAL ERROR: Inconsistency between conditionIndex enum and ALL_CONDITIONS");
  }

  const std::vector<std::string>& Node::ALL_CONDITIONS()
  {
    if (s_allConditions.empty())
      initAllConditions();
    return s_allConditions;
  }

  const std::string& Node::getConditionName(size_t idx)
  {
    return ALL_CONDITIONS()[idx];
  }
  
  Node::ConditionIndex Node::getConditionIndex(char const *cName)
  {
    std::vector<std::string> const &allConds = Node::ALL_CONDITIONS();
    for (size_t i = 0; i < allConds.size(); ++i)
      if (allConds[i] == cName)
        return (Node::ConditionIndex) i;
    return conditionIndexMax;
  }

  Node::Node(char const *nodeId, Node *parent)
    : NodeConnector(),
      ExpressionListener(),
      QueueItem<Node>(),
      m_queueStatus(0),
      m_state(INACTIVE_STATE),
      m_outcome(NO_OUTCOME),
      m_failureType(NO_FAILURE),
      m_nextState(NO_NODE_STATE),
      m_nextOutcome(NO_OUTCOME),
      m_nextFailureType(NO_FAILURE),
      m_parent(parent),
      m_conditions(),
      m_stateVariable(*this),
      m_outcomeVariable(*this),
      m_failureTypeVariable(*this),
      m_nodeId(nodeId),
      m_transitionTimes(),
      m_transitionStates(),
      m_traceIdx(0),
      m_garbageConditions(),
      m_cleanedConditions(false),
      m_cleanedVars(false)
  {
    debugMsg("Node:node", " Constructor for \"" << m_nodeId << "\"");
    commonInit();
  }

  // Used only by module test
  Node::Node(const std::string& type, 
             const std::string& name, 
             NodeState state,
             Node *parent)
    : NodeConnector(),
      ExpressionListener(),
      QueueItem<Node>(),
      m_queueStatus(0),
      m_state(state),
      m_outcome(NO_OUTCOME),
      m_failureType(NO_FAILURE),
      m_nextState(NO_NODE_STATE),
      m_nextOutcome(NO_OUTCOME),
      m_nextFailureType(NO_FAILURE),
      m_parent(parent),
      m_conditions(),
      m_stateVariable(*this),
      m_outcomeVariable(*this),
      m_failureTypeVariable(*this),
      m_nodeId(name),
      m_transitionTimes(),
      m_transitionStates(),
      m_traceIdx(0),
      m_garbageConditions(),
      m_cleanedConditions(false), 
      m_cleanedVars(false)
  {
    commonInit();

    for (size_t i = 0; i < conditionIndexMax; ++i) {
      Expression *expr = new BooleanVariable(false);
      debugMsg("Node:node",
               " Created internal variable "
               << ALL_CONDITIONS()[i] <<
               " with value FALSE for node " << m_nodeId);
      m_conditions[i] = expr;
      m_garbageConditions[i] = true;
      if (i != preIdx && i != postIdx && getCondition(i))
        getCondition(i)->addListener(this);
    }

    PlexilNodeType nodeType = parseNodeType(type.c_str());
    // Activate the conditions required by the provided state
    switch (m_state) {

    case INACTIVE_STATE:
      break;

    case WAITING_STATE:
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      activateExitCondition();
      activateInvariantCondition();
      activatePreSkipStartConditions();
      break;

    case EXECUTING_STATE:
      activateAncestorExitInvariantConditions();
      activateExitCondition();
      activateInvariantCondition();
      activateEndCondition();
      activatePostCondition();
      break;

    case FAILING_STATE:
      assertTrueMsg(nodeType != NodeType_Empty,
                    "Node module test constructor: FAILING state invalid for Empty nodes");
      // Defer to subclass
      break;

    case FINISHING_STATE:
      assertTrueMsg(nodeType != NodeType_Empty,
                    "Node module test constructor: FINISHING state invalid for Empty nodes");
      // Defer to subclass
      break;

    case ITERATION_ENDED_STATE:
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      activateRepeatCondition();
      break;

    case FINISHED_STATE:
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL, "Node module test constructor: Invalid state " << state);
    }
  }

  // N.B.: called from base class constructor
  void Node::commonInit() {
    debugMsg("Node:node", " common initialization");
    if (m_parent)
      m_variablesByName.setParentMap(m_parent->getChildVariableMap());

    // Initialize transition trace
    logTransition(g_interface->currentTime(), (NodeState) m_state);
  }

  NodeVariableMap *Node::getChildVariableMap()
  {
    return NULL;
  }

  bool Node::addVariable(char const *name, Expression *var)
  {
    if (m_variablesByName.find(name) != m_variablesByName.end())
      return false; // duplicate
    m_variablesByName[name] = var;
    return true;
  }

  bool Node::addLocalVariable(char const *name, Expression *var)
  {
    if (!addVariable(name, var))
      return false;
    m_localVariables.push_back(var);
    return true;
  }

  void Node::finalizeConditions()
  {
    // Create conditions that may wrap user-defined conditions
    createConditionWrappers();

    //
    // *** N.B. ***
    // This should be the only place where listeners are added to conditions.
    //

    // Add listeners to local conditions
    for (size_t condIdx = skipIdx; condIdx < conditionIndexMax; ++condIdx)
      switch (condIdx) {

      case postIdx:
      case preIdx:
        break; // these conditions don't need listeners

      default:
        if (m_conditions[condIdx])
          m_conditions[condIdx]->addListener(this);
        break;
      }

    // Attach listeners to ancestor invariant and ancestor end conditions
    // Root node doesn't need them because the default conditions are constants
    if (m_parent) {
      Expression *ancestorCond = getAncestorExitCondition();
      if (ancestorCond)
        ancestorCond->addListener(this);

      ancestorCond = getAncestorInvariantCondition();
      if (ancestorCond)
        ancestorCond->addListener(this);

      ancestorCond = getAncestorEndCondition();
      if (ancestorCond)
        ancestorCond->addListener(this);
    }
  }

  void Node::addUserCondition(ConditionIndex which, Expression *cond, bool isGarbage)
  {
    assertTrue_2(which >= skipIdx && which <= repeatIdx,
                 "Invalid condition index for user condition");
    checkParserException(!m_conditions[which],
                         "Duplicate " << getConditionName(which) << " for Node \"" << m_nodeId << "\"");
    m_conditions[which] = cond;
    m_garbageConditions[which] = isGarbage;
  }

  // Default method
  void Node::createConditionWrappers()
  {
  }

  Node::~Node() 
  {
    debugMsg("Node:~Node", " base class destructor for " << m_nodeId);

    // Remove conditions as they may refer to variables, either ours or another node's
    // Derived classes' destructors should also call this
    cleanUpConditions();

    // cleanUpNodeBody(); // NOT USEFUL here - derived classes MUST call this!

    // Now safe to delete variables
    cleanUpVars();
  }

  void Node::cleanUpConditions() 
  {
    if (m_cleanedConditions)
      return;

    debugMsg("Node:cleanUpConditions", " for " << m_nodeId);

    // Remove condition listeners
    for (size_t i = 0; i < conditionIndexMax; ++i) {
      Expression *cond = getCondition(i);
      if (cond)
        cond->removeListener(this);
    }

    // Clean up conditions
    // N.B.: Ancestor-end and ancestor-invariant MUST be cleaned up before
    // end and invariant, respectively. 
    for (size_t i = 0; i < conditionIndexMax; ++i) {
      if (m_garbageConditions[i]) {
        debugMsg("Node:cleanUpConds",
                 "<" << m_nodeId << "> Removing condition " << getConditionName(i));
        delete m_conditions[i];
      }
      m_conditions[i] = NULL;
      m_garbageConditions[i] = false;
    }

    m_cleanedConditions = true;
  }

  // Default method.
  void Node::cleanUpNodeBody()
  {
  }

  // Called from base class destructor and possibly derived as well.
  void Node::cleanUpVars() 
  {
    if (m_cleanedVars)
      return;
    checkError(m_cleanedConditions,
               "Have to clean up variables before conditions can be cleaned.");

    debugMsg("Node:cleanUpVars", " for " << m_nodeId);

    // Clear map
    m_variablesByName.clear();

    // Delete user-spec'd variables
    for (std::vector<Expression *>::iterator it = m_localVariables.begin(); it != m_localVariables.end(); ++it) {
      debugMsg("Node:cleanUpVars",
               "<" << m_nodeId << "> Removing " << **it);
      delete (Expression *) (*it);
    }
    m_localVariables.clear();

    // Delete internal variables
    m_cleanedVars = true;
  }

  // Make the node (and its children, if any) active.
  void Node::activate()
  {
    // Activate conditions needed for INACTIVE state
    transitionToInactive();

    // Other initializations as required by node type
    specializedActivate();
  }

  // Default method
  void Node::specializedActivate()
  {
  }

  Expression *Node::getCondition(size_t idx)
  {
    switch (idx) {

    case ancestorEndIdx:
    case ancestorExitIdx:
    case ancestorInvariantIdx:
      if (m_parent)
        return m_parent->m_conditions[idx];
      else
        return NULL;

    default:
      return m_conditions[idx];
    }
  }

  Expression const *Node::getCondition(size_t idx) const
  {
    switch (idx) {

    case ancestorEndIdx:
    case ancestorExitIdx:
    case ancestorInvariantIdx:
      if (m_parent)
        return m_parent->m_conditions[idx];
      else
        return NULL;

    default:
      return m_conditions[idx];
    }
  }

  // Default methods.
  std::vector<Node *>& Node::getChildren()
  {
    static std::vector<Node *> sl_emptyNodeVec;
    return sl_emptyNodeVec;
  }

  const std::vector<Node *>& Node::getChildren() const
  {
    static const std::vector<Node *> sl_emptyNodeVec;
    return sl_emptyNodeVec;
  }

  /**
   * @brief Notifies the node that one of its conditions has changed.
   * @note Renamed from conditionChanged.
   */
  void Node::notifyChanged(Expression const * /* src */)
  {
    g_exec->notifyNodeConditionChanged(this);
  }

  /**
   * @brief Gets the destination state of this node, were it to transition, based on the values of various conditions.
   * @return True if the new destination state is different from the last check, false otherwise.
   * @note Sets m_nextState, m_nextOutcome, m_nextFailureType as a side effect.
   */
  bool Node::getDestState() 
  {
    debugMsg("Node:getDestState",
             "Getting destination state for " << m_nodeId << " from state " <<
             getStateName());

    // clear this for sake of unit test
    m_nextState = NO_NODE_STATE;

    switch (m_state) {
    case INACTIVE_STATE:
      return getDestStateFromInactive();

    case WAITING_STATE:
      return getDestStateFromWaiting();

    case EXECUTING_STATE:
      return getDestStateFromExecuting();

    case FINISHING_STATE:
      return getDestStateFromFinishing();

    case FINISHED_STATE:
      return getDestStateFromFinished();

    case FAILING_STATE:
      return getDestStateFromFailing();

    case ITERATION_ENDED_STATE:
      return getDestStateFromIterationEnded();

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Node::getDestState: invalid node state " << m_state);
      return false;
    }
  }

  //
  // State transition logic
  //

  void Node::transition(double time) 
  {
    checkError(m_nextState != NO_NODE_STATE
               && m_nextState != m_state,
               "Attempted to transition node " << m_nodeId <<
               " when it is ineligible.");

    debugMsg("Node:transition", "Transitioning '" << m_nodeId
             << "' from " << nodeStateName(m_state)
             << " to " << nodeStateName(m_nextState)
             << " at " << std::setprecision(15) << time);
    
    transitionFrom();
    transitionTo(time);

    // Clear pending-transition variables
    m_nextState = NO_NODE_STATE;
    m_nextOutcome = NO_OUTCOME;
    m_nextFailureType = NO_FAILURE;

    condDebugMsg((m_state == FINISHED_STATE),
                 "Node:outcome",
                 "Outcome of '" << m_nodeId <<
                 "' is " << outcomeName((NodeOutcome) m_outcome));
    condDebugMsg((m_state == FINISHED_STATE && m_outcome == FAILURE_OUTCOME),
                 "Node:failure",
                 "Failure type of '" << m_nodeId <<
                 "' is " << failureTypeName((FailureType) m_failureType));
    condDebugMsg((m_state == ITERATION_ENDED_STATE),
                 "Node:iterationOutcome",
                 "Outcome of '" << m_nodeId <<
                 "' is " << outcomeName((NodeOutcome) m_outcome));
  }

  // Common method 
  void Node::transitionFrom()
  {
    switch (m_state) {
    case INACTIVE_STATE:
      transitionFromInactive();
      break;

    case WAITING_STATE:
      transitionFromWaiting();
      break;

    case EXECUTING_STATE:
      transitionFromExecuting();
      break;

    case FINISHING_STATE:
      transitionFromFinishing();
      break;

    case FINISHED_STATE:
      transitionFromFinished();
      break;

    case FAILING_STATE:
      transitionFromFailing();
      break;

    case ITERATION_ENDED_STATE:
      transitionFromIterationEnded();
      break;

    default:
      checkError(ALWAYS_FAIL,
                 "Node::transitionFrom: Invalid node state " << m_state);
    }
  }

  // Common method 
  void Node::transitionTo(double time)
  {
    switch (m_nextState) {
    case INACTIVE_STATE:
      transitionToInactive();
      break;

    case WAITING_STATE:
      transitionToWaiting();
      break;

    case EXECUTING_STATE:
      transitionToExecuting();
      break;

    case FINISHING_STATE:
      transitionToFinishing();
      break;

    case FINISHED_STATE:
      transitionToFinished();
      break;

    case FAILING_STATE:
      transitionToFailing();
      break;

    case ITERATION_ENDED_STATE:
      transitionToIterationEnded();
      break;

    default:
      checkError(ALWAYS_FAIL,
                 "Node::transitionTo: Invalid destination state " << m_nextState);
    }

    setState((NodeState) m_nextState, time);
    if (m_nextOutcome != NO_OUTCOME) {
      setNodeOutcome((NodeOutcome) m_nextOutcome);
      if (m_nextFailureType != NO_FAILURE) 
        setNodeFailureType((FailureType) m_nextFailureType);
    }
    if (m_nextState == EXECUTING_STATE)
      execute();
  }

  //
  // INACTIVE
  //
  // Description and methods here apply to ALL nodes
  //
  // Start state
  // Legal predecessor states: FINISHED
  // Conditions active: If parent in EXECUTING - AncestorExit, AncestorEnd, AncestorInvariant,
  //                    else none
  // Legal successor states: WAITING, FINISHED

  // Common method
  void Node::transitionToInactive()
  {
  }

  // Common method
  bool Node::getDestStateFromInactive()
  {
    if (m_parent) {
      switch (m_parent->getState()) {

      case FINISHED_STATE:
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. Parent state == FINISHED.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;

      case EXECUTING_STATE: {
        // N.B. Ancestor-exit, ancestor-invariant, ancestor-end should have been activated by parent
        bool temp;
        Expression *cond;
        if ((cond = getAncestorExitCondition())) {
          checkError(cond->isActive(),
                     "Node::getDestStateFromInactive: Ancestor exit for " << m_nodeId << " is inactive.");
          if (cond->getValue(temp) && temp) {
            debugMsg("Node:getDestState",
                     " '" << m_nodeId << "' destination: FINISHED. Parent EXECUTING and ANCESTOR_EXIT_CONDITION true.");
            m_nextState = FINISHED_STATE;
            m_nextOutcome = SKIPPED_OUTCOME;
            return true;
          }
        }

        if ((cond = getAncestorInvariantCondition())) {
          checkError(cond->isActive(),
                     "Node::getDestStateFromInactive: Ancestor invariant for " << m_nodeId << " is inactive.");
          if (cond->getValue(temp) && !temp) {
            debugMsg("Node:getDestState",
                     " '" << m_nodeId << "' destination: FINISHED. Parent EXECUTING and ANCESTOR_INVARIANT_CONDITION false.");
            m_nextState = FINISHED_STATE;
            m_nextOutcome = SKIPPED_OUTCOME;
            return true;
          }
        }

        if ((cond = getAncestorEndCondition())) {
          checkError(cond->isActive(),
                     "Node::getDestStateFromInactive: Ancestor end for " << m_nodeId << " is inactive.");
          if (cond->getValue(temp) && temp) {
            debugMsg("Node:getDestState",
                     " '" << m_nodeId << "' destination: FINISHED. Parent EXECUTING and ANCESTOR_END_CONDITION true.");
            m_nextState = FINISHED_STATE;
            m_nextOutcome = SKIPPED_OUTCOME;
            return true;
          }
        }

        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: WAITING. Parent state == EXECUTING.");
        m_nextState = WAITING_STATE;
        return true;
      }

      default:
        debugMsg("Node:getDestState", 
                 " '" << m_nodeId << "' destination: no state.");
        return false;
      }
    }
    else {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: WAITING. Root node.");
      m_nextState = WAITING_STATE;
      return true;
    }
  }

  // Common method
  void Node::transitionFromInactive()
  {
    if (m_nextState == WAITING_STATE) {
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      return;
    }
    // Only other legal transition is to FINISHED,
    // in which case no action is required.
    checkError(m_nextState == FINISHED_STATE,
               "Attempting to transition from INACTIVE to invalid state '"
               << nodeStateName(m_nextState) << "'");
  }

  //
  // WAITING
  //
  // Description and methods here apply to ALL nodes.
  //
  // Legal predecessor states: INACTIVE, ITERATION_ENDED
  // Conditions active: AncestorEnd, AncestorExit, AncestorInvariant, Exit, Pre, Skip, Start
  // Legal successor states: EXECUTING, FINISHED, ITERATION_ENDED

  // ** N.B. Preceding state must ensure that AncestorEnd, AncestorExit, and AncestorInvariant are active.

  // Common method
  void Node::transitionToWaiting()
  {
    activateExitCondition();
    activatePreSkipStartConditions();
  }

  // Common method
  bool Node::getDestStateFromWaiting()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Exit condition for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. EXIT_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Ancestor invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_INVARIANT_CONDITION false.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;
      }
    }

    if ((cond = getAncestorEndCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Ancestor end for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_END_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;
      }
    }

    if ((cond = getSkipCondition())) {
      checkError(cond->isActive(), 
                 "Node::getDestStateFromWaiting: Skip for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. SKIP_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = SKIPPED_OUTCOME;
        return true;
      }
    }

    if ((cond = getStartCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Start for " << m_nodeId << " is inactive.");
      if (!cond->getValue(temp) || !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: no state. START_CONDITION false or unknown");
        return false;
      }
    }
    if ((cond = getPreCondition()) && (!cond->getValue(temp) || !temp)) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromWaiting: Pre for " << m_nodeId << " is inactive.");
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: ITERATION_ENDED. START_CONDITION true and PRE_CONDITION false or unknown.");
      m_nextState = ITERATION_ENDED_STATE;
      m_nextOutcome = FAILURE_OUTCOME;
      m_nextFailureType = PRE_CONDITION_FAILED;
      return true;
    }
    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: EXECUTING. START_CONDITION and PRE_CONDITION are both true.");
    m_nextState = EXECUTING_STATE;
    return true;
  }

  // Common method
  void Node::transitionFromWaiting()
  {
    deactivatePreSkipStartConditions();
    switch (m_nextState) {

    case EXECUTING_STATE:
      deactivateAncestorEndCondition();
      break;

    case FINISHED_STATE:
      deactivateAncestorExitInvariantConditions();
      deactivateAncestorEndCondition();
      // fall through
    case ITERATION_ENDED_STATE:
      deactivateExitCondition();
      break;

    default:
      checkError(ALWAYS_FAIL,
                 "Attempting to transition from WAITING to invalid state '"
                 << nodeStateName(m_nextState) << "'");
      break;
    }
  }

  //
  // EXECUTING 
  // 
  // Description and methods here are for Empty node only
  //
  // Legal predecessor states: WAITING
  // Conditions active: AncestorExit, AncestorInvariant, End, Exit, Invariant, Post
  // Legal successor states: FINISHED, ITERATION_ENDED

  // Default method
  void Node::transitionToExecuting()
  {
    activateInvariantCondition();
    activateEndCondition();
    activatePostCondition();
  }

  // Default method
  bool Node::getDestStateFromExecuting()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Exit condition for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: ITERATION_ENDED. EXIT_CONDITION true.");
        m_nextState = ITERATION_ENDED_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = EXITED;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Ancestor invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. Ancestor invariant false.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = PARENT_FAILED;
        return true;
      }
    }

    if ((cond = getInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: ITERATION_ENDED. Invariant false.");
        m_nextState = ITERATION_ENDED_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    if ((cond = getEndCondition()) && (!cond->getValue(temp) || !temp)) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: End for " << m_nodeId << " is inactive.");
      return false;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: ITERATION_ENDED. End condition .");
    m_nextState = ITERATION_ENDED_STATE;
    if ((cond = getPostCondition()) && (!cond->getValue(temp) || !temp)) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Post for " << m_nodeId << " is inactive.");
      m_nextOutcome = FAILURE_OUTCOME;
      m_nextFailureType = POST_CONDITION_FAILED;
    }
    else
      m_nextOutcome = SUCCESS_OUTCOME;
    return true;
  }

  // Empty node method
  void Node::transitionFromExecuting()
  {
    checkError(this->getType() == NodeType_Empty,
               "Expected empty node, got " << nodeTypeString(this->getType()));

    deactivateExitCondition();
    deactivateInvariantCondition();
    deactivateEndCondition();
    deactivatePostCondition();
    if (m_nextState == FINISHED_STATE) {
      deactivateAncestorExitInvariantConditions();
    }
    else if (m_nextState == ITERATION_ENDED_STATE) {
      activateAncestorEndCondition();
    }
    else checkError(ALWAYS_FAIL,
                    "Attempting to transition empty node from EXECUTING to invalid state '"
                    << nodeStateName(m_nextState) << "'");

    deactivateExecutable();
  }

  //
  // ITERATION_ENDED
  //
  // Description and methods here apply to ALL nodes
  //
  // Legal predecessor states: EXECUTING, FAILING, FINISHING, WAITING
  // Conditions active: AncestorEnd, AncestorExit, AncestorInvariant, Repeat
  // Legal successor states: FINISHED, WAITING

  // *** N.B.: Preceding state must ensure that AncestorEnd, AncestorExit, and AncestorInvariant are active!

  // Common method
  void Node::transitionToIterationEnded() 
  {
    activateRepeatCondition();
  }

  // Common method
  bool Node::getDestStateFromIterationEnded()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromIterationEnded: Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromIterationEnded: Ancestor invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_INVARIANT false.");
        m_nextState = FINISHED_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = PARENT_FAILED;
        return true;
      }
    }

    if ((cond = getAncestorEndCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromIterationEnded: Ancestor end for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. ANCESTOR_END true.");
        m_nextState = FINISHED_STATE;
        // outcome, failure type already set
        return true;
      }
    }

    if ((cond = getRepeatCondition())) {
      if (!cond->getValue(temp)) {
        checkError(cond->isActive(),
                   "Node::getDestStateFromIterationEnded: Repeat for " << m_nodeId << " is inactive.");
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: no state. ANCESTOR_END false or unknown and REPEAT unknown.");
        return false;
      } 
      if (temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: WAITING. REPEAT_CONDITION true.");
        m_nextState = WAITING_STATE;
        return true;
      }
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: FINISHED. REPEAT_CONDITION false.");
    m_nextState = FINISHED_STATE;
    return true;
  }

  // Common method
  void Node::transitionFromIterationEnded()
  {
    deactivateRepeatCondition();

    if (m_nextState == FINISHED_STATE) {
      deactivateAncestorExitInvariantConditions();
      deactivateAncestorEndCondition();
    }
    else if (m_nextState == WAITING_STATE) {
      reset();
    }
    else
      checkError(ALWAYS_FAIL,
                 "Attempting to transition from ITERATION_ENDED to invalid state '"
                 << nodeStateName(m_nextState) << "'");

  }

  //
  // FINISHED
  //
  // Description and methods here apply to ALL nodes
  //
  // Legal predecessor states: EXECUTING, FAILING, FINISHING, INACTIVE, ITERATION_ENDED, WAITING
  // Conditions active:
  // Legal successor states: INACTIVE

  // Common method
  void Node::transitionToFinished()
  {
  }

  // Common method
  bool Node::getDestStateFromFinished()
  {
    if (m_parent && m_parent->getState() == WAITING_STATE) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: INACTIVE.  Parent state == WAITING.");
      m_nextState = INACTIVE_STATE;
      return true;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: no state.");
    return false;
  }

  // Common method
  void Node::transitionFromFinished()
  {
    checkError(m_nextState == INACTIVE_STATE,
               "Attempting to transition from FINISHED to invalid state '"
               << nodeStateName(m_nextState) << "'");
    reset();
  }

  //
  // FINISHING (legal for ListNode and LibraryCallNode only)
  //
  // Description and methods here apply to all other node types.
  //
  // Legal predecessor states: n/a
  // Conditions active: n/a
  // Legal successor states: n/a

  // Default method
  void Node::transitionToFinishing()
  {
    checkError(ALWAYS_FAIL,
               "No transition to FINISHING state defined for this node");
  }

  // Default method
  bool Node::getDestStateFromFinishing()
  {
    checkError(ALWAYS_FAIL,
               "Attempted to compute destination state from FINISHING for node " << m_nodeId
               << " of type " << getType());
    return false;
  }

  // Default method
  void Node::transitionFromFinishing()
  {
    checkError(ALWAYS_FAIL,
               "No transition from FINISHING state defined for this node");
  }

  //
  // FAILING (legal for Command, Update, ListNode, and LibraryCallNode only)
  //
  // Description and methods here apply to Empty and Assignment nodes.
  //
  // Legal predecessor states: n/a
  // Conditions active: n/a
  // Legal successor states: n/a

  // Default method
  void Node::transitionToFailing()
  {
    checkError(ALWAYS_FAIL,
               "No transition to FAILING state defined for this node");
  }

  // Default method
  bool Node::getDestStateFromFailing()
  {
    checkError(ALWAYS_FAIL,
               "Attempted to compute destination state from FAILING for node " << m_nodeId
               << " of type " << getType());
    return false;
  }

  // Default method
  void Node::transitionFromFailing()
  {
    checkError(ALWAYS_FAIL,
               "No transition from FAILING state defined for this node");
  }

  // ***
  // *** END NODE STATE LOGIC ***
  // ***

  std::string const &Node::getStateName() const {
    return nodeStateName(m_state);
  }

  NodeState Node::getState() const {
    return (NodeState) m_state;
  }

  // Some transition handlers call this twice.
  void Node::setState(NodeState newValue, double tym) // FIXME
  {
    if (newValue == m_state)
      return;
    checkError(newValue <= nodeStateMax(),
               "Attempted to set an invalid NodeState value for this node");
    m_state = newValue;
    logTransition(tym, newValue);
    m_stateVariable.changed();
    if (m_state == FINISHED_STATE && !m_parent)
      // Mark this node as ready to be deleted -
      // with no parent, it cannot be reset, therefore cannot transition again.
      g_exec->markRootNodeFinished(this); // puts node on exec's finished queue
    else
      notifyChanged(&m_stateVariable); // check for potential of additional transitions
  }

  //
  // Transition time trace methods
  //

  void Node::logTransition(double tym, NodeState newState)
  {
    if (newState == INACTIVE_STATE)
      // either just constructed or parent repeating; clear history
      m_traceIdx = 0;
    else if (newState == WAITING_STATE && m_traceIdx > 1)
      // this node is repeating
      m_traceIdx = 1;
    else {
      assertTrue_2(m_traceIdx < NODE_STATE_MAX,
                   "Node transition trace is full");
    }
    m_transitionTimes[m_traceIdx] = tym;
    m_transitionStates[m_traceIdx] = newState;
    ++m_traceIdx;
  }

  bool Node::getStateTransitionTime(NodeState state,
                                    bool isEnd,
                                    double &result) const
  {
    if (!m_traceIdx)
      return false; // buffer is empty
    size_t i = 0;
    // Find the entry for that state (i.e. start time)
    for (; i < m_traceIdx; ++i)
      if (m_transitionStates[i] == state)
        break;
    if (i == m_traceIdx)
      return false; // state not found
    if (isEnd) {
      if (++i == m_traceIdx)
        return false; // current state, hasn't ended yet
    }
    result = m_transitionTimes[i];
    return true;
  }

  double Node::getCurrentStateStartTime() const
  {
    if (!m_traceIdx)
      return -DBL_MAX; // buffer empty
    return m_transitionTimes[m_traceIdx - 1];
  }

  void Node::setNodeOutcome(NodeOutcome o)
  {
    checkError(o >= NO_OUTCOME && o < OUTCOME_MAX,
               "Node::setNodeOutcome: invalid outcome value");
    m_outcome = o;
    m_outcomeVariable.changed();
  }

  NodeOutcome Node::getOutcome() const
  {
    return (NodeOutcome) m_outcome;
  }

  void Node::setNodeFailureType(FailureType f)
  {
    checkError(f >= NO_FAILURE && f < FAILURE_TYPE_MAX,
               "Node::setNodeFailureType: invalid failureType value");
    m_failureType = f;
    m_failureTypeVariable.changed();
  }

  FailureType Node::getFailureType() const
  {
    return (FailureType) m_failureType;
  }

  // Searches ancestors' maps when required
  Expression *Node::findVariable(char const *name)
  {
    debugMsg("Node:findVariable",
             " for node '" << m_nodeId
             << "', searching by name for \"" << name << "\"");
    Expression *result = m_variablesByName.findVariable(name);
    if (result) {
      debugMsg("Node:findVariable",
               " Returning " << result->toString());
      return result;
    }
    return NULL;
  }

  Expression *Node::findLocalVariable(char const *name)
  {
    NodeVariableMap::const_iterator it = m_variablesByName.find(name);
    if (it != m_variablesByName.end()) {
      debugMsg("Node:findLocalVariable",
               " " << m_nodeId << " Returning " << it->second->toString());
      return it->second;
    }
    else {
      debugMsg("Node:findLocalVariable", " " << m_nodeId << " " << name << " not found");
      return NULL;
    }
  }

  // Default methods
  Node const *Node::findChild(char const * /* childName */) const
  {
    return NULL;
  }

  Node *Node::findChild(char const * /* childName */)
  {
    return NULL;
  }

  //
  // Conditions
  //

  // These are special because parent owns the condition expression
  void Node::activateAncestorEndCondition()
  {
  }
  void Node::activateAncestorExitInvariantConditions()
  {
  }

  // User conditions
  void Node::activatePreSkipStartConditions()
  {
    Expression *temp;
    if ((temp = m_conditions[skipIdx]))
      temp->activate();
    if ((temp = m_conditions[startIdx]))
      temp->activate();
    if ((temp = m_conditions[preIdx]))
      temp->activate();
  }

  void Node::activateEndCondition()
  {
    if (m_conditions[endIdx])
      m_conditions[endIdx]->activate();
  }

  void Node::activateExitCondition()
  {
    if (m_conditions[exitIdx])
      m_conditions[exitIdx]->activate();
  }

  void Node::activateInvariantCondition()
  {
    if (m_conditions[invariantIdx])
      m_conditions[invariantIdx]->activate();
  }

  void Node::activatePostCondition()
  {
    if (m_conditions[postIdx])
      m_conditions[postIdx]->activate();
  }

  void Node::activateRepeatCondition()
  {
    if (m_conditions[repeatIdx])
      m_conditions[repeatIdx]->activate();
  }

  // These are for specialized node types
  void Node::activateActionCompleteCondition()
  {
    checkError(m_conditions[actionCompleteIdx],
               "No ActionCompleteCondition exists in node \"" << m_nodeId << "\"");
    m_conditions[actionCompleteIdx]->activate();
  }

  void Node::activateAbortCompleteCondition()
  {
    checkError(m_conditions[abortCompleteIdx],
               "No AbortCompleteCondition exists in node \"" << m_nodeId << "\"");
    m_conditions[abortCompleteIdx]->activate();
  }

  // These are special because parent owns the condition expression
  void Node::deactivateAncestorEndCondition()
  {
  }
  void Node::deactivateAncestorExitInvariantConditions()
  {
  }

  // User conditions
  void Node::deactivatePreSkipStartConditions()
  {
    Expression *temp;
    if ((temp = m_conditions[skipIdx]))
      temp->deactivate();
    if ((temp = m_conditions[startIdx]))
      temp->deactivate();
    if ((temp = m_conditions[preIdx]))
      temp->deactivate();
  }

  void Node::deactivateEndCondition()
  {
    if (m_conditions[endIdx])
      m_conditions[endIdx]->deactivate();
  }

  void Node::deactivateExitCondition()
  {
    if ((m_conditions[exitIdx]))
      m_conditions[exitIdx]->deactivate();
  }

  void Node::deactivateInvariantCondition()
  {
    if ((m_conditions[invariantIdx]))
      m_conditions[invariantIdx]->deactivate();
  }

  void Node::deactivatePostCondition()
  {
    if ((m_conditions[postIdx]))
      m_conditions[postIdx]->deactivate();
  }

  void Node::deactivateRepeatCondition()
  {
    if (m_conditions[repeatIdx])
      m_conditions[repeatIdx]->deactivate();
  }

  // These are for specialized node types
  void Node::deactivateActionCompleteCondition()
  {
    checkError(m_conditions[actionCompleteIdx],
               "No ActionCompleteCondition exists in node \"" << m_nodeId << "\"");
    m_conditions[actionCompleteIdx]->deactivate();
  }

  void Node::deactivateAbortCompleteCondition()
  {
    checkError(m_conditions[abortCompleteIdx],
               "No AbortCompleteCondition exists in node \"" << m_nodeId << "\"");
    m_conditions[abortCompleteIdx]->deactivate();
  }

  void Node::execute() 
  {
    checkError(m_state == EXECUTING_STATE,
               "Node \"" << m_nodeId
               << "\" told to handle execution, but it's in state '" <<
               getStateName() << "'");
    debugMsg("Node:execute", "Executing node " << m_nodeId);

    // Activate local variables
    for (std::vector<Expression *>::iterator vit = m_localVariables.begin();
         vit != m_localVariables.end();
         ++vit)
      (*vit)->activate();

    // legacy message for unit test
    debugMsg("PlexilExec:handleNeedsExecution",
             "Storing action for node '" << m_nodeId <<
             "' of type '" << nodeTypeString(this->getType()) << 
             "' to be executed.");

    specializedHandleExecution();
  }

  // default method
  void Node::specializedHandleExecution()
  {
  }

  void Node::reset()
  {
    debugMsg("Node:reset", "Re-setting node " << m_nodeId);

    //reset outcome and failure type
    m_outcome = NO_OUTCOME;
    m_failureType = NO_FAILURE;

    for (std::vector<Expression *>::const_iterator it = m_localVariables.begin();
         it != m_localVariables.end();
         ++it)
      if ((*it)->isAssignable())
        (*it)->asAssignable()->reset();

    specializedReset();
  }

  // Default method
  void Node::specializedReset()
  {
  }

  // Default method
  void Node::abort() 
  {
    checkError(ALWAYS_FAIL, "Abort illegal for node type " << getType());
  }

  void Node::deactivateExecutable() 
  {
    specializedDeactivateExecutable();
    // Deactivate local variables
    for (std::vector<Expression *>::iterator vit = m_localVariables.begin();
         vit != m_localVariables.end();
         ++vit)
      (*vit)->deactivate();
  }

  // Default method
  void Node::specializedDeactivateExecutable()
  {
  }

  std::string Node::toString(const unsigned int indent)
  {
    std::ostringstream retval;
    print(retval, indent);
    return retval.str();
  }

  std::ostream& operator<<(std::ostream& stream, const Node& node)
  {
    node.print(stream, 0);
    return stream;
  }

  void Node::print(std::ostream& stream, const unsigned int indent) const
  {
    std::string indentStr(indent, ' ');

    stream << indentStr << m_nodeId << "{\n";
    stream << indentStr << " State: " << getStateName() <<
      " (" << getCurrentStateStartTime() << ")\n";
    if (m_state == FINISHED_STATE) {
      stream << indentStr << " Outcome: " << outcomeName((NodeOutcome) m_outcome) << '\n';
      if (m_failureType != NO_FAILURE)
        stream << indentStr << " Failure type: " <<
          failureTypeName((FailureType) m_failureType) << '\n';
      // Print variables, starting with command handle
      printCommandHandle(stream, indent);
      printVariables(stream, indent);
    }
    else if (m_state != INACTIVE_STATE) {
      // Print conditions
      for (size_t i = 0; i < conditionIndexMax; ++i) {
        if (getCondition(i))
        stream << indentStr << " " << getConditionName(i) << ": "
               << getCondition(i)->toString()
               << '\n';
      }
      // Print variables, starting with command handle
      printCommandHandle(stream, indent);
      printVariables(stream, indent);
    }
    // print children
    for (std::vector<Node *>::const_iterator it = getChildren().begin(); it != getChildren().end(); ++it)
      (*it)->print(stream, indent + 2);
    stream << indentStr << "}" << std::endl;
  }

  // Default method does nothing
  void Node::printCommandHandle(std::ostream& /* stream */, const unsigned int /* indent */) const
  {
  }

  // Print variables
  void Node::printVariables(std::ostream& stream, const unsigned int indent) const
  {
    std::string indentStr(indent, ' ');
    for (NodeVariableMap::const_iterator it = m_variablesByName.begin();
         it != m_variablesByName.end();
         ++it) {
      stream << indentStr << " " << it->first << ": " <<
        *(it->second) << '\n';
    }
  }

}