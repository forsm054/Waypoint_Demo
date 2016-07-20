<?xml version="1.0" encoding="UTF-8"?><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator" FileName="plans/FourSteps1.ple"><GlobalDeclarations LineNo="36" ColNo="8"><CommandDeclaration LineNo="36" ColNo="8"><Name>QueryRobotState</Name><Return><Name>_return_0</Name><Type>Real</Type><MaxSize>3</MaxSize></Return><Parameter><Name>name</Name><Type>String</Type></Parameter></CommandDeclaration><CommandDeclaration LineNo="37" ColNo="8"><Name>Move</Name><Return><Name>_return_0</Name><Type>Integer</Type></Return><Parameter><Name>name</Name><Type>String</Type></Parameter><Parameter><Name>direction</Name><Type>Integer</Type></Parameter></CommandDeclaration></GlobalDeclarations><Node NodeType="NodeList" epx="Sequence" LineNo="41" ColNo="1"><NodeId>FourSteps</NodeId><VariableDeclarations><DeclareVariable LineNo="40" ColNo="1"><Name>RobotName</Name><Type>String</Type><InitialValue><StringValue>RobotYellow</StringValue></InitialValue></DeclareVariable><DeclareVariable LineNo="41" ColNo="1"><Name>StartX</Name><Type>Real</Type></DeclareVariable><DeclareVariable LineNo="41" ColNo="1"><Name>StartY</Name><Type>Real</Type></DeclareVariable><DeclareVariable LineNo="41" ColNo="1"><Name>EndX</Name><Type>Real</Type></DeclareVariable><DeclareVariable LineNo="41" ColNo="1"><Name>EndY</Name><Type>Real</Type></DeclareVariable><DeclareVariable LineNo="42" ColNo="1"><Name>Index</Name><Type>Integer</Type><InitialValue><IntegerValue>0</IntegerValue></InitialValue></DeclareVariable></VariableDeclarations><PostCondition><AND><EQNumeric><RealVariable>StartX</RealVariable><RealVariable>EndX</RealVariable></EQNumeric><EQNumeric><RealVariable>StartY</RealVariable><RealVariable>EndY</RealVariable></EQNumeric></AND></PostCondition><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">GetStartPosition</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">GetStartPosition</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">MoveLoop</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">MoveLoop</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">GetEndPosition</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">GetEndPosition</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="NodeList" epx="Sequence" LineNo="48" ColNo="2"><NodeId>GetStartPosition</NodeId><VariableDeclarations><DeclareArray LineNo="47" ColNo="2"><Name>RobotState</Name><Type>Real</Type><MaxSize>3</MaxSize></DeclareArray></VariableDeclarations><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">GetRobotState</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">GetRobotState</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ASSIGNMENT__1</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">ASSIGNMENT__1</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ASSIGNMENT__2</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">ASSIGNMENT__2</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="Command" LineNo="51" ColNo="16"><NodeId>GetRobotState</NodeId><EndCondition><IsKnown><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>0</IntegerValue></Index></ArrayElement></IsKnown></EndCondition><NodeBody><Command><ArrayVariable>RobotState</ArrayVariable><Name><StringValue>QueryRobotState</StringValue></Name><Arguments LineNo="52" ColNo="32"><StringVariable>RobotName</StringVariable></Arguments></Command></NodeBody></Node><Node NodeType="Assignment" LineNo="54" ColNo="2"><NodeId>ASSIGNMENT__1</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">GetRobotState</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><RealVariable>StartX</RealVariable><NumericRHS><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>0</IntegerValue></Index></ArrayElement></NumericRHS></Assignment></NodeBody></Node><Node NodeType="Assignment" LineNo="55" ColNo="2"><NodeId>ASSIGNMENT__2</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">ASSIGNMENT__1</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><RealVariable>StartY</RealVariable><NumericRHS><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>1</IntegerValue></Index></ArrayElement></NumericRHS></Assignment></NodeBody></Node></NodeList></NodeBody></Node><Node NodeType="NodeList" epx="Sequence" LineNo="59" ColNo="2"><NodeId>MoveLoop</NodeId><VariableDeclarations><DeclareArray LineNo="58" ColNo="2"><Name>Directions</Name><Type>Integer</Type><MaxSize>4</MaxSize><InitialValue><IntegerValue>3</IntegerValue><IntegerValue>0</IntegerValue><IntegerValue>1</IntegerValue><IntegerValue>2</IntegerValue></InitialValue></DeclareArray></VariableDeclarations><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">GetStartPosition</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><RepeatCondition><LT><IntegerVariable>Index</IntegerVariable><IntegerValue>4</IntegerValue></LT></RepeatCondition><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">Move</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">Move</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ASSIGNMENT__4</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">ASSIGNMENT__4</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="Command" LineNo="65" ColNo="21"><NodeId>Move</NodeId><VariableDeclarations><DeclareVariable LineNo="62" ColNo="12"><Name>result</Name><Type>Integer</Type></DeclareVariable></VariableDeclarations><EndCondition><IsKnown><IntegerVariable>result</IntegerVariable></IsKnown></EndCondition><PostCondition><EQNumeric><IntegerVariable>result</IntegerVariable><IntegerValue>1</IntegerValue></EQNumeric></PostCondition><NodeBody><Command><IntegerVariable>result</IntegerVariable><Name><StringValue>Move</StringValue></Name><Arguments LineNo="66" ColNo="26"><StringVariable>RobotName</StringVariable><ArrayElement><ArrayVariable>Directions</ArrayVariable><Index><IntegerVariable>Index</IntegerVariable></Index></ArrayElement></Arguments></Command></NodeBody></Node><Node NodeType="Assignment" LineNo="68" ColNo="2"><NodeId>ASSIGNMENT__4</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">Move</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><IntegerVariable>Index</IntegerVariable><NumericRHS><ADD LineNo="68" ColNo="16"><IntegerVariable>Index</IntegerVariable><IntegerValue>1</IntegerValue></ADD></NumericRHS></Assignment></NodeBody></Node></NodeList></NodeBody></Node><Node NodeType="NodeList" epx="Sequence" LineNo="72" ColNo="2"><NodeId>GetEndPosition</NodeId><VariableDeclarations><DeclareArray LineNo="71" ColNo="2"><Name>RobotState</Name><Type>Real</Type><MaxSize>3</MaxSize></DeclareArray></VariableDeclarations><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">MoveLoop</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">GetRobotState</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">GetRobotState</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ASSIGNMENT__6</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">ASSIGNMENT__6</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">ASSIGNMENT__7</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">ASSIGNMENT__7</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="Command" LineNo="75" ColNo="16"><NodeId>GetRobotState</NodeId><EndCondition><IsKnown><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>0</IntegerValue></Index></ArrayElement></IsKnown></EndCondition><NodeBody><Command><ArrayVariable>RobotState</ArrayVariable><Name><StringValue>QueryRobotState</StringValue></Name><Arguments LineNo="76" ColNo="32"><StringVariable>RobotName</StringVariable></Arguments></Command></NodeBody></Node><Node NodeType="Assignment" LineNo="78" ColNo="2"><NodeId>ASSIGNMENT__6</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">GetRobotState</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><RealVariable>EndX</RealVariable><NumericRHS><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>0</IntegerValue></Index></ArrayElement></NumericRHS></Assignment></NodeBody></Node><Node NodeType="Assignment" LineNo="79" ColNo="2"><NodeId>ASSIGNMENT__7</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">ASSIGNMENT__6</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Assignment><RealVariable>EndY</RealVariable><NumericRHS><ArrayElement><ArrayVariable>RobotState</ArrayVariable><Index><IntegerValue>1</IntegerValue></Index></ArrayElement></NumericRHS></Assignment></NodeBody></Node></NodeList></NodeBody></Node></NodeList></NodeBody></Node></PlexilPlan>