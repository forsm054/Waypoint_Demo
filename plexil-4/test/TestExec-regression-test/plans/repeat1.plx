<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp --><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList">
      <NodeId>Root</NodeId>
      <VariableDeclarations>
         <DeclareVariable>
            <Name>count</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>0</IntegerValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <EndCondition>
         <EQInternal>
            <NodeStateVariable>
               <NodeId>Inc</NodeId>
            </NodeStateVariable>
            <NodeStateValue>FINISHED</NodeStateValue>
         </EQInternal>
      </EndCondition>
      <StartCondition>
         <BooleanValue>1</BooleanValue>
      </StartCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment">
               <NodeId>Inc</NodeId>
               <RepeatCondition>
                  <LT>
                     <IntegerVariable>count</IntegerVariable>
                     <IntegerValue>4</IntegerValue>
                  </LT>
               </RepeatCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>count</IntegerVariable>
                     <NumericRHS>
                        <ADD>
                           <IntegerValue>1</IntegerValue>
                           <IntegerVariable>count</IntegerVariable>
                        </ADD>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>