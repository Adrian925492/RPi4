# About device tree

The device tree is a simple tree structured description of hardware. The structure is described as nodes. The nodes can contain node properties or subnodes. Properties of the nodes are dreated in form of pairs key - value. All is written to a text files with .dts extention.

Each node represents piece of hardware.

An example device tree:
```
/dts-v1/;

/ {
    node1 {
        a-string-property = "A string";
        a-string-list-property = "first string", "second string";
        // hex is implied in byte arrays. no '0x' prefix is required
        a-byte-data-property = [01 23 34 56];
        child-node1 {
            first-child-property;
            second-child-property = <1>;
            a-string-property = "Hello, world";
        };
        child-node2 {
        };
    };
    node2 {
        an-empty-property;
        a-cell-property = <1 2 3 4>; /* each number (cell) is a uint32 */
        child-node1 {
        };
    };
};
```
Each node is described inside `{...};` paranthesis, as in C-style format. 

The root node, which is master node for each other nodes in a tree is represented by `/` symbol.

The properwies can be represented by several type of data:
-> String property, ex. `name = "sone string property";
-> Cell property - a 32bit intigers delimited by space, like a tuple, ex. `name = < 123 0x23 0xabcd>;`
-> Binary property - a list of data, ex`name = [0x01 0x02 0x03 0x04];`
-> Combined type property - set of data with different types can be assigned to a single key using "," separator, ex: `name = "string", <1 2 3>, [0x01 0x02 0x03 0x04];`
-> Lists of strings - set of data with only string types can be written with coma "," separators as well, ex:`name = "str1", "str2";`

Each property nas a `;` sign at the end - like in C-style structure.

### Root node required entries

* compatible - the compatible entry used for root node identifies the base microcontroler that is used. In that case, compatible string has a form of `compatible="manufacturer,model";` 
 
```
  compatible = "stm,stm32f7";
```

* cpu - the cpu property identifies cpu cores existing on the machine. It is numered from zero to last one, after @ sign. Each cpu needs also compatible string, which identifies what kind of core it is. Here, like in root node compatible string, we have property data in form of "manufacturer,core type".

```
  cpus {
    cpu@0 {
      compatible = "arm,cortex-a9";
    };
    cpu@1 {
      compatible = "arm,cortex-a9";
    }
 };
```

### Nodes naming

The node names are in general any user defined string. It is good practice to name the node according to what the node handhelds.

The node name is in form `name[@<unit-address>]`

The name is always requied. The address is given only when the node represents some hardware that can be mapped to memory region, like gpio controller that uses registers of some specified adress. The adress here is than the firts address for the hardware described by the node, and is given when the node has reg property. The value given here is not parsed, it is just part of a name, but giving the address here is a good practice.

If we have nodes that uses addreses, it is usuall to use the same node name for many nodes, as long as all the nodes has different addresses given.

The `@` property can be also used for numering the nodes, like in cpu nodes example above.

### Nodes hierarchy

In general, the hierarchy of the nodes represents how the hardware pieces are connected to the system. The root node represents the CPU - it is the core node.

For example, if we have some external i2c temperature sensor, which is connected threw i2c bus, the i2c bus node would be subnode of the root node and the sensor would be child node of the i2c, as it is connected to i2c, like in the schema:

    CPU <-> I2C <-> Temp sensor
    
 The example device tree of such case would be (each string here is imagined!):
 
 ```
 /{
    compatible="stm,stm32f4";
    i2c@ff000000 {
        compatible = "stm-i2c";
        ...
        temperature_snsor {
            compatible = "i2c, temp_sensor";
        };
    };
};
```
        

## Device tree compiling

## Device tree entries

! Every device node has a `compatible` property

* compatible - the key, from which the system knows which driver shall be used for the device. That property, in general, gives an information what kind of device is connected to the node. Compatible property is a list of strings. The strings are coma separated, like `"string1", "string2"`. The next strings represents other drivers that the node is compatible with. Taht, in general, allows some particular device use some particular drivers in its context. It is not allowed to use any wildcards in compatible property.
* reg - that key is applicable only for addresable devices, means that devices that can be somehow mapped into cpu memory space, for example internal piece of memory, controllers etc. The reg property defines memory regions that the device uses. It makes the driver allowed to use that memory regions. The prperty is written in form `reg = <adress1 lenght1 [address2 lenght2] [...] >`. There is at least one pair of data in the cell - the adress, which identifies memory region from which the controller will start to be accesible, and the range, means how many pieces of memory will be accesible. The next pairs (address2 and range2 and so on) are optional and can be used if we want the node to have an access to defragmented pieces of memory.

The `reg` property can also be written in form of `reg = <address>`. That means it represents situation, when the node has an access to a single adress register.

* #address-cells - if informs, how large is each property given in `reg` property. In 32but machines, giving that paramter to 1 menas, that each entry in reg cell property is an uint32 type, so in case we have `reg=<1, 2>`, the 1 and 2 will be treated as uint32 values. If we need some extedned values to be passed by reg, we can increase the #address-cells property. Thus, the values given in the reg cells will be treated as multiplications of base architecture word size. For example, on 32bit machine with `#address-cells=<2>`, the 1 and 2 of the example would be treated as u64 values.
* #size-cells - It informs what is the lenght of reg cell tuple, means how mny entries it can have. For example, if we have single register accesed node (like a cpu from previous example), that property will have value of 1. If we have just single memory region to map, it will be 2, but if we have 3 memory regions to use, we will have that value equal to 6, example `#size-cells=<6>;`

Special case - some devices uses the external memory range than the cpu, but the memory can be used and accesed from linux. In that case, we ca use #size-cells equal to 3. In general, than the `reg` parameter will be like:
```
external-bus {
    #adress-cells = <2>;
    #size-cells=<3>;

    flash@X,0 {
        reg = <X address range>;
    };
}
```
where X can represent a chip-select pin encoded into address space for the external memory mapped to linux.
