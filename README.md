<h4 align="center">
    <img src="assets/mdv.svg" align="center" width="100"/>
</h4>


<h4 align="center">
    <img src="https://img.shields.io/travis/LinArcX/mdv"/>  <img src="https://img.shields.io/github/tag/LinArcX/mdv.svg?colorB=green"/>  <img src="https://img.shields.io/github/repo-size/LinArcX/mdv.svg"/>  <img src="https://img.shields.io/github/languages/top/LinArcX/mdv.svg"/>
</h4>

## Installation
### Void
`sudo xbps-install -S mdv`

### Arch
- git version

`trizen -S mdv-git`

- release version

`trizen -S mdv`

### Build From Source
First Install these dependencies first:

#### Dependencies
runtime dependencies:
- glibc

build dependencies:
- cmake

Then clone and build the project:

```
git clone https://github.com/LinArcX/mdv/
cd mdv; mkdir build; cd build
cmake ..; make
```

And finally, run it:

`cd ..; build/mdv`


## Usage
    ./markdown [input-file] [output-file]
If you don't specify the _output-file_, program will parse everything to stdout in your terminal.

###Examples

#### Quotes
    > quotes
    > quotes

#### Headers
    # 1st header
    ## 2nd header
    ### 3rd header
    .
    .
    .
    and so on.

#### Inline formating:
    *bold test* and _italics_.

#### Links:
    [link-name](example.com/cats)

#### Lists:
    * No 1
    * Next please!
    * Here I am!

## License
![License](https://img.shields.io/github/license/LinArcX/mdv.svg)
