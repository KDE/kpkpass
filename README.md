# KPkPass

Library to deal with Apple Wallet pass files.

## File Format

Apple Wallet files are essentially ZIP files containing a JSON description of the pass,
translated message catalogs and graphical assets to render the pass.

See [Apple's documentation](https://developer.apple.com/library/content/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/TopLevel.html)
about this.

## API

KPkPass provides means to access the content of Apple Wallet files, both via a C++ API
and a QML-compatible property interface.

The entry point in both cases is KPkPass::Pass to load an existing pass.

