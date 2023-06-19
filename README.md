# DBMS final project: Adaptive Hybrid B+ tree
The project is implementation of paper [adaptive hybrid index](https://dl.acm.org/doi/abs/10.1145/3514221.3526121). The implementation is based on [tlx b+ tree](https://github.com/tlx/tlx/tree/master/tlx/container).  We implementation B+ tree that use universal packed encoding and that use hybrid encoding. The modification is marked by ```//! Hybrid index``` in ```btree/include/hybrid_tree.hpp```. The adaptation manager logic's is located in ``` btree/include/AdaptationManager.hpp```

### Building

```
cd scripts
sh pre_install.sh
cd ..
cmake .
make
```

### Test

We support three type of encoding migration choice:
* 0: gapped and packed
* 1: gapped and succinct
* 2: gapped and packed, disabling node expansion
* 3: gapped, packed and succinct

You sholud specify the number in mode
```
./baseline workloads/sample_big
./hybrid workloads/sample_big 0
./hybrid workloads/sample_big 1
./hybrid workloads/sample_big 2
./hybrid workloads/sample_big 3

```

### Sample Usage
```
typedef tlx::btree_map<string, string, less<string> btree_type;
btree_type *bt = new btree_type();
bt->setMode(mode);
bt->insert2(key, value);
bt->find(key);
```

### Note

Currently we doesn't support inseration if there are some succinct leaf nodes in the tree.


