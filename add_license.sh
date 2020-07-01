#!/bin/bash

find "$@" -name '*.h' -o -name '*.cpp' -o -name '*.qml' | grep -v /3rdparty/ | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} " "$FILE" ; then continue; fi
    thisfile=`basename $FILE`
    authorName=`git config user.name`
    authorEmail=`git config user.email`
    thisYear=`date +%Y`
    cat <<EOF > "$FILE".tmp
/*
    SPDX-FileCopyrightText: $thisYear $authorName <$authorEmail>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done
