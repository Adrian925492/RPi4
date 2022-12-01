#About device tree

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
-> Binary property - a list of data, ex`name = [0x01 0x02 0x03 0x04];
-> Combined type property - set of data with different types can be assigned to a single key using "," separator, ex: `name = "string", <1 2 3>, [0x01 0x02 0x03 0x04];
-> Lists of strings - set of data with only string types can be written with coma "," separators as well, ex:`name = "str1", "str2";

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

##Device tree compiling

##Device tree entries

