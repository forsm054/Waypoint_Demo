<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp --><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="Command">
      <NodeId>Root</NodeId>
      <VariableDeclarations>
         <DeclareVariable>
            <Name>p</Name>
            <Type>Real</Type>
         </DeclareVariable>
      </VariableDeclarations>
      <NodeBody>
         <Command>
            <ResourceList>
               <Resource>
                  <ResourceName>
                     <StringValue>Memory</StringValue>
                  </ResourceName>
                  <ResourcePriority>
                     <RealValue>2.3</RealValue>
                  </ResourcePriority>
                  <ResourceLowerBound>
                     <RealValue>10.0</RealValue>
                  </ResourceLowerBound>
                  <ResourceUpperBound>
                     <RealValue>20.0</RealValue>
                  </ResourceUpperBound>
               </Resource>
            </ResourceList>
            <RealVariable>p</RealVariable>
            <Name>
               <StringValue>store</StringValue>
            </Name>
         </Command>
      </NodeBody>
   </Node>
</PlexilPlan>