<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="Command">
      <NodeId>root</NodeId>
      <StartCondition>
         <LookupOnChange>
            <Name>
               <StringValue>Foo</StringValue>
            </Name>
            <Tolerance>
               <RealValue>1.0</RealValue>
            </Tolerance>
            <Arguments>
               <RealValue>2.3</RealValue>
            </Arguments>
         </LookupOnChange>
      </StartCondition>
      <InvariantCondition>
         <LookupNow>
            <Name>
               <StringValue>Bar</StringValue>
            </Name>
            <Arguments>
               <RealValue>3.0</RealValue>
            </Arguments>
         </LookupNow>
      </InvariantCondition>
      <NodeBody>
         <Command>
            <Name>
               <LookupNow>
                  <Name>
                     <StringValue>CommandName</StringValue>
                  </Name>
               </LookupNow>
            </Name>
            <Arguments>
               <LookupNow>
                  <Name>
                     <StringValue>CommandArg1</StringValue>
                  </Name>
               </LookupNow>
               <LookupNow>
                  <Name>
                     <StringValue>CommandArg2</StringValue>
                  </Name>
               </LookupNow>
            </Arguments>
         </Command>
      </NodeBody>
   </Node>
</PlexilPlan>