# Aurdino Template

A simple template with a few debug and build headers

## Installation

Download the files into a folder and open it as a vscode workspace.  
Note: The git commands and [c_cpp_properties.json](.vscode/c_cpp_properties.json) contain Windows specific commands.

#### Git

```dos
mkdir tmp
cd tmp
git clone https://github.com/calw20/Arduino-Template.git
git checkout-index -a -f --prefix=./../
cd ..
rmdir /S /Q "./tmp/"
```

#### Svn

```dos
svn export https://github.com/calw20/Arduino-Template/trunk/
```

## License

[MIT](LICENSE)
