<?xml version="1.0" encoding="UTF-8"?><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator" FileName="./basic/AddArray.ple"><Node NodeType="NodeList" epx="Sequence" LineNo="3" ColNo="2"><NodeId>Root</NodeId><VariableDeclarations><DeclareArray LineNo="2" ColNo="2"><Name>foo</Name><Type>Real</Type><MaxSize>4</MaxSize><InitialValue><RealValue>0.0</RealValue><RealValue>0.0</RealValue><RealValue>0.0</RealValue><RealValue>0.0</RealValue></InitialValue></DeclareArray><DeclareVariable LineNo="3" ColNo="2"><Name>temp</Name><Type>Real</Type><InitialValue><RealValue>0.0</RealValue></InitialValue></DeclareVariable></VariableDeclarations><InvariantCondition><NOT><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">while__0</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">while__0</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="NodeList" epx="While" LineNo="5" ColNo="2"><NodeId>while__0</NodeId><RepeatCondition><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ep2cp_WhileTest</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal></RepeatCondition><NodeBody><NodeList><Node NodeType="Empty" epx="Condition"><NodeId>ep2cp_WhileTest</NodeId><PostCondition><LT><RealVariable>temp</RealVariable><RealValue>3.0</RealValue></LT></PostCondition></Node><Node NodeType="NodeList" epx="Action" LineNo="7" ColNo="4"><NodeId>BLOCK__1</NodeId><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">SimpleAssignment</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">SimpleAssignment</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">SimpleArrayAssignment</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">SimpleArrayAssignment</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">SimpleArrayAssignment2</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">SimpleArrayAssignment2</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><StartCondition><EQInternal><NodeOutcomeVariable><NodeRef dir="sibling">ep2cp_WhileTest</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal></StartCondition><SkipCondition><AND><EQInternal><NodeStateVariable><NodeRef dir="sibling">ep2cp_WhileTest</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal><EQInternal><NodeFailureVariable><NodeRef dir="sibling">ep2cp_WhileTest</NodeRef></NodeFailureVariable><NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue></EQInternal></AND></SkipCondition><NodeBody><NodeList><Node NodeType="Assignment" LineNo="7" ColNo="24"><NodeId>SimpleAssignment</NodeId><NodeBody><Assignment><RealVariable>temp</RealVariable><NumericRHS><ArrayElement><Name>foo</Name><Index><IntegerValue>1</IntegerValue></Index></ArrayElement></NumericRHS></Assignment></NodeBody></Node><Node NodeType="Assignment" LineNo="8" ColNo="29"><NodeId>SimpleArrayAssignment</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">SimpleAssignment</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><ArrayElement><Name>foo</Name><Index><IntegerValue>1</IntegerValue></Index></ArrayElement><NumericRHS><ADD LineNo="8" ColNo="45"><ArrayElement><Name>foo</Name><Index><IntegerValue>1</IntegerValue></Index></ArrayElement><RealValue>1.0</RealValue></ADD></NumericRHS></Assignment></NodeBody></Node><Node NodeType="Assignment" LineNo="9" ColNo="30"><NodeId>SimpleArrayAssignment2</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">SimpleArrayAssignment</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><ArrayElement><Name>foo</Name><Index><IntegerValue>2</IntegerValue></Index></ArrayElement><NumericRHS><ADD LineNo="9" ColNo="46"><ArrayElement><Name>foo</Name><Index><IntegerValue>2</IntegerValue></Index></ArrayElement><RealValue>2.0</RealValue></ADD></NumericRHS></Assignment></NodeBody></Node></NodeList></NodeBody></Node></NodeList></NodeBody></Node></NodeList></NodeBody></Node></PlexilPlan>