# Documentation for ION Engine
ION Engine is using [Doxygen](https://doxygen.nl/) to automatically generate documentation from the source code.

## Dependencies
The following tools are required to generate the complete API reference as is.
* Doxygen (https://doxygen.nl/)
* Graphviz (https://graphviz.org/)

## Setup
After Doxygen and Graphviz has been installed, make sure that the dot path in `Doxyfile` is set to where Graphviz\bin is located.

```
DOT_PATH               = "C:\Program Files\Graphviz\bin"
```

Open `Doxyfile` with doxywizard, choose `Run` and click `Run doxygen`. The documentation will be generated to `output`.