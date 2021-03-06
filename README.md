# Arduino Template  

[//]: # (Licensed under MIT, see LICENSE for more details)
[//]: # (Cal.W 2020)

A simple template with a few debug and build headers

## Installation

Download the files into a folder and open it as a vscode workspace.  
Note: The git commands and [c_cpp_properties.json](.vscode/c_cpp_properties.json) contain Windows specific commands.

### Git

```dos
mkdir tmp
cd tmp
git clone https://github.com/calw20/Arduino-Template.git
git checkout-index -a -f --prefix=./../
cd ..
rmdir /S /Q "./tmp/"
```

### Svn

```dos
svn export https://github.com/calw20/Arduino-Template/trunk/
```

### Uploading Arduino Code without the IDE

The powershell script [Upload](Upload.ps1) will automatically download the Arduino-CLI, download set libraries,
run a pre-build command and upload the set file all with one click!
>:notebook_with_decorative_cover: Note: You may need to run `Set-ExecutionPolicy RemoteSigned` or equivalent in an_administrator_ powershell before the script can be run.

### Contributing and Further setup instructions

For further in-depth information regarding workspace setup and contributing to this template please see the [Setup](SETUP.md) and [Contributing](CONTRIBUTING.md) files respectively.

## License & Notes

Do what you want with this project. I made it mostly to further my knowledge in
its respective felids, so it is most likely going to be a jumbled, barely
functional, glob of what I hope can be called code. Hence I can't promise it
wont break, cry, give you up or create a rift in time and space. Sorry.
So if you do end up using this, please let me know as I would love to see how its
used. Now if for some reason you feel like this has helped you in some useful way
and you manage to spot me out in the world, feel free to buy me a drink (coffee or
beer is always nice) but I highly doubt this will have been useful enough for that.
Enjoy and good luck!

### License

[MIT](LICENSE)
