#!/bin/sh
# Create git tag
#
# SPDX-FileCopyrightText: 2024 Johnny Jazeix <jazeix@gmail.com>
#
#   SPDX-License-Identifier: GPL-3.0-or-later

function usage() {
    echo "Usage: ./tools/create_release.sh -v <version> [-r <date>] [-h]"
    echo "  -v, --version <version>   contains the new version <major>.<minor>.<patch>"
    echo "  -r, --release <date>      contains the date of the release YYYY-MM-DD (optional)"
    echo "  -h, --help                displays this help"
}

if [ ! -f io.github.petitlapin.Li-ri.appdata.xml ]
then
    echo "ERROR: Run me from the top level source dir"
    exit 1
fi

while [[ $# -gt 0 ]]; do
  case $1 in
    -v|--version)
      VERSION="$2"
      shift # past argument
      shift # past value
      ;;
    -r|--release)
      DATE="$2"
      shift # past argument
      shift # past value
      ;;
    -h|--help)
        usage;
        exit 0
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

if [[ ! "${VERSION}" ]]
then
    echo "Mission version"
    usage
    exit 1
fi

major=`echo $VERSION | cut -d. -f1`
minor=`echo $VERSION | cut -d. -f2`
patch=`echo $VERSION | cut -d. -f3`

version=${major}.${minor}.${patch}
echo "Version  = $version"
echo "Date     = ${DATE}"

./tools/bump_version.sh -v ${VERSION} -c

# TODO
if [[ "${CHANGELOG}" ]]
then
    git tag -a v${VERSION} -m "create tag v${VERSION}"
fi
