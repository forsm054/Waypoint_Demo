<?xml version="1.0" encoding="UTF-8"?><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator" FileName="plans/RoboSimSimple.ple"><GlobalDeclarations LineNo="28" ColNo="8"><CommandDeclaration LineNo="28" ColNo="8"><Name>Move</Name><Return><Name>_return_0</Name><Type>Integer</Type></Return><Parameter><Name>name</Name><Type>String</Type></Parameter><Parameter><Name>direction</Name><Type>Integer</Type></Parameter></CommandDeclaration><CommandDeclaration LineNo="29" ColNo="8"><Name>MoveRandom</Name><Return><Name>_return_0</Name><Type>Integer</Type></Return><Parameter><Name>name</Name><Type>String</Type></Parameter></CommandDeclaration><CommandDeclaration LineNo="30" ColNo="8"><Name>QueryEnergySensor</Name><Return><Name>_return_0</Name><Type>Real</Type><MaxSize>5</MaxSize></Return><Parameter><Name>name</Name><Type>String</Type></Parameter></CommandDeclaration></GlobalDeclarations><Node NodeType="NodeList" epx="Sequence" LineNo="34" ColNo="1"><NodeId>RoboSimSimple</NodeId><VariableDeclarations><DeclareVariable LineNo="33" ColNo="1"><Name>RobotName</Name><Type>String</Type><InitialValue><StringValue>RobotYellow</StringValue></InitialValue></DeclareVariable><DeclareVariable LineNo="34" ColNo="1"><Name>randNumber</Name><Type>Integer</Type><InitialValue><IntegerValue>0</IntegerValue></InitialValue></DeclareVariable></VariableDeclarations><InvariantCondition><NOT><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">MoveRobot</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">MoveRobot</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="NodeList" epx="Sequence" LineNo="39" ColNo="2"><NodeId>MoveRobot</NodeId><RepeatCondition><EQInternal><NodeOutcomeVariable><NodeId>NodeWrapper</NodeId></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal></RepeatCondition><InvariantCondition><NOT><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">NodeWrapper</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">NodeWrapper</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="NodeList" epx="Sequence" LineNo="42" ColNo="3"><NodeId>NodeWrapper</NodeId><PostCondition><AND><EQInternal><NodeOutcomeVariable><NodeId>Move</NodeId></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal><EQInternal><NodeOutcomeVariable><NodeId>QueryEnergySensor</NodeId></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal><EQInternal><NodeOutcomeVariable><NodeId>MoveRandom</NodeId></NodeOutcomeVariable><NodeOutcomeValue>SUCCESS</NodeOutcomeValue></EQInternal></AND></PostCondition><InvariantCondition><NOT><OR><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">Move</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">Move</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">QueryEnergySensor</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">QueryEnergySensor</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND><AND><EQInternal><NodeOutcomeVariable><NodeRef dir="child">MoveRandom</NodeRef></NodeOutcomeVariable><NodeOutcomeValue>FAILURE</NodeOutcomeValue></EQInternal><EQInternal><NodeStateVariable><NodeRef dir="child">MoveRandom</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></AND></OR></NOT></InvariantCondition><NodeBody><NodeList><Node NodeType="Command" LineNo="50" ColNo="10"><NodeId>Move</NodeId><VariableDeclarations><DeclareVariable LineNo="47" ColNo="4"><Name>res</Name><Type>Integer</Type></DeclareVariable></VariableDeclarations><EndCondition><IsKnown><IntegerVariable>res</IntegerVariable></IsKnown></EndCondition><PostCondition><EQNumeric><IntegerVariable>res</IntegerVariable><IntegerValue>1</IntegerValue></EQNumeric></PostCondition><NodeBody><Command><IntegerVariable>res</IntegerVariable><Name><StringValue>Move</StringValue></Name><Arguments LineNo="51" ColNo="15"><StringVariable>RobotName</StringVariable><IntegerVariable>randNumber</IntegerVariable></Arguments></Command></NodeBody></Node><Node NodeType="Command" LineNo="56" ColNo="13"><NodeId>QueryEnergySensor</NodeId><VariableDeclarations><DeclareArray LineNo="55" ColNo="4"><Name>energy</Name><Type>Real</Type><MaxSize>5</MaxSize></DeclareArray></VariableDeclarations><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">Move</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Command><ArrayVariable>energy</ArrayVariable><Name><StringValue>QueryEnergySensor</StringValue></Name><Arguments LineNo="57" ColNo="31"><StringVariable>RobotName</StringVariable></Arguments></Command></NodeBody></Node><Node NodeType="Command" LineNo="60" ColNo="16"><NodeId>MoveRandom</NodeId><StartCondition><EQInternal><NodeStateVariable><NodeRef dir="sibling">QueryEnergySensor</NodeRef></NodeStateVariable><NodeStateValue>FINISHED</NodeStateValue></EQInternal></StartCondition><NodeBody><Command><IntegerVariable>randNumber</IntegerVariable><Name><StringValue>MoveRandom</StringValue></Name><Arguments LineNo="61" ColNo="27"><StringVariable>RobotName</StringVariable></Arguments></Command></NodeBody></Node></NodeList></NodeBody></Node></NodeList></NodeBody></Node></NodeList></NodeBody></Node></PlexilPlan>