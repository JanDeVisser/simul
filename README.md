# A Simple Ben Eater Inspired 8-bit computer

Please describe me.

### Register IDs

 ID      | Register       | R W | Remarks                                    
---------|----------------|-----|--------------------------------------------
 0000 00 | A              | x x |
 0001 01 | B              | x x |
 0010 02 | C              | x x |
 0011 03 | D              | x x |
 0100 04 | ALU LHS/Result | x x | Put: LHS; Get: Result                      
 0101 05 | ALU Operation  | x x | Put: RHS with OP0-3: Operation; Get: Flags 
 0110 06 | Instruction    | - x |
 0111 07 | Mem            | x x |
 1000 08 | Prog Counter   | x x |
 1001 09 | SP             | x x |
 1010 10 | Si             | x x |
 1011 11 | Di             | x x |
 1100 12 | TX             |     |
 1101 13 |                |     |
 1110 14 | Monitor        | x - |
 1111 15 | Mem Addr       | - x |

### Control Lines

 Pin   | Name                   | Description                                                     
-------|------------------------|-----------------------------------------------------------------
 1     | `GND`                  | Common Ground                                                   
 2     | `VCC`                  | +5V Power                                                       
 3     | `CLK`                  | Square Wave Clock                                               
 4     | ~~`CLK`~~              | Reverse Clock                                                   
 5     | `^CLK`                 | "Burst" Clock at rising edge of CLK                             
 6     | `HLT`                  | Halts clock                                                     
 7     | ~~`SUS`~~              | Requests Control Unit to cease dispatching instructions.        
 8     | ~~`XDATA`~~            | 8 bit Data transfer instruction                                 
 9     | ~~`XADDR`~~            | 16 bit address transfer instruction                             
 10    | ~~`SACK`~~             | Set by Control Unit as acknowlegement of SUS.                   
 11-14 | `OP0`-`OP3`            | Custom Operation codes used by various instructions. See below. 
 15    | `RST`                  | Reset                                                           
 16    | ~~`IO`~~               | In/Out instruction                                              
 17-20 | `PUT0`-`PUT3`          | Register ID of the target of a transfer instruction.            
 21-24 | `GET0`-`GET3`          | Register ID of the source of a transfer instruction.            
 25-32 | `D0`-`D7`              | Databus                                                         
 33-40 | `A0`-`A7` / `A8`-`A15` | Address bus                                                     

### Operation Codes

#### XADDR

 OP0-3  | Function                        
--------|---------------------------------
 `XX01` | Increment source (Get) register 
 `XX10` | Decrement source (Get) register 

#### XDATA

_When moving data between 8- and 16-bit registers:_

 OP0-3  | Function                                        
--------|-------------------------------------------------
 `XXX0` | Transfer to/from the LSB of the 16-bit register 
 `XXX1` | Transfer to/from the MSB of the 16-bit register 

_When the target register is ALU Operation (`0x5`):_

 OP0-3        | Operation                    
--------------|------------------------------
 `0x0` `0000` | `CLR`                        
 `0x1` `0001` | `RHS - LHS`                  
 `0x2` `0010` | `LHS - RHS`                  
 `0x3` `0011` | `LHS + RHS`                  
 `0x4` `0100` | `XOR`                        
 `0x5` `0101` | `OR`                         
 `0x6` `0110` | `AND`                        
 `0x7` `0111` | `0xFF`                       
 `0x8` `1000` | `CLR`                        
 `0x9` `1001` | `RHS - LHS` with borrow      
 `0xA` `1010` | `LHS - RHS` with borrow      
 `0xB` `1011` | `LHS + RHS` with carry       
 `0xC` `1100` | `SHL` (no carry shifting in) 
 `0xD` `1101` | `SHL` with carry             
 `0xE` `1110` | `SHR` (no carry shifting in) 
 `0xF` `1111` | `SHR` with carry             

The shift operations work on the value being `PUT`; the value latched in the
`LHS` register is not used.

Logical invert (`NOT`) can be achieved by first doing a `PUT` to `0x5` with
operation `0x7` resulting in `0xFF` in the result latch. Then load the value
to be inverted into `LHS` using a `PUT` of `0x4` followed by a `XOR` with the
result latch using `GET` `0x4`/`PUT` `0x5` with operation `0x4`.

## N O T E S
