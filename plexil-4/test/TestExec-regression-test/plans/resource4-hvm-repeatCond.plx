<?xml version="1.0" encoding="UTF-8"?>
<!-- The priority ordering is as follows: -->
<PlexilPlan>
  <GlobalDeclarations>
    <CommandDeclaration>
      <Name>c1</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
      <Parameter>
        <Type>Integer</Type>
      </Parameter>
    </CommandDeclaration>
    <CommandDeclaration>
      <Name>c2</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
    </CommandDeclaration>
    <CommandDeclaration>
      <Name>c3</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
    </CommandDeclaration>
  </GlobalDeclarations>
  <Node NodeType="NodeList">
    <NodeId>SimpleTask</NodeId>
    <NodeBody>
      <NodeList>
        <Node NodeType="Command">
          <NodeId>C1</NodeId>
          <EndCondition>
            <EQNumeric>
              <IntegerValue>10</IntegerValue>
              <IntegerVariable>returnValue</IntegerVariable>
            </EQNumeric>
          </EndCondition>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>C1</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <VariableDeclarations>
            <DeclareVariable>
              <Name>returnValue</Name>
              <Type>Integer</Type>
              <InitialValue>
                <IntegerValue>-1</IntegerValue>
              </InitialValue>
            </DeclareVariable>
            <DeclareVariable>
              <Name>head_priority</Name>
              <Type>Integer</Type>
              <InitialValue>
                <IntegerValue>20</IntegerValue>
              </InitialValue>
            </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>head</StringValue>
                  </ResourceName>
                  <ResourcePriority>
                    <IntegerVariable>head_priority</IntegerVariable>
                  </ResourcePriority>
                </Resource>
              </ResourceList>
              <IntegerVariable>returnValue</IntegerVariable>
              <Name><StringValue>c1</StringValue></Name>
              <Arguments>
                <IntegerValue>20</IntegerValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>C2</NodeId>
          <VariableDeclarations>
            <DeclareVariable>
              <Name>v_priority2</Name>
              <Type>Integer</Type>
              <InitialValue>
                <IntegerValue>25</IntegerValue>
              </InitialValue>
            </DeclareVariable>
          </VariableDeclarations>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>C2</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <RepeatCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>C2</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
            </EQInternal>
          </RepeatCondition>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>vision</StringValue>
                  </ResourceName>
                  <ResourcePriority>
                    <IntegerVariable>v_priority2</IntegerVariable>
                  </ResourcePriority>
                </Resource>
              </ResourceList>
              <Name><StringValue>c2</StringValue></Name>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>C3</NodeId>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>C3</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <VariableDeclarations>
            <DeclareVariable>
              <Name>mem_priority2</Name>
              <Type>Integer</Type>
              <InitialValue>
                <IntegerValue>30</IntegerValue>
              </InitialValue>
            </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>memory</StringValue>
                  </ResourceName>
                  <ResourcePriority>
                    <IntegerVariable>mem_priority2</IntegerVariable>
                  </ResourcePriority>
                  <ResourceLowerBound>
                    <RealValue>0.3</RealValue>
                  </ResourceLowerBound>
                  <ResourceUpperBound>
                    <RealValue>0.3</RealValue>
                  </ResourceUpperBound>
                </Resource>
              </ResourceList>
              <Name><StringValue>c3</StringValue></Name>
            </Command>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>