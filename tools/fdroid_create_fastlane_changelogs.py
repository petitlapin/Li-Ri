#!/usr/bin/python3
#
# GCompris - fdroid_create_fastlane_changelogs.py
#
# SPDX-FileCopyrightText: 2024 Johnny Jazeix <jazeix@gmail.com>
#
#   SPDX-License-Identifier: GPL-3.0-or-later

import sys
import yaml

from packaging import version


def main(argv):
    if argv[0] != "./tools/fdroid_create_fastlane_changelogs.py":
        print("Needs to be run from top level of Li-Ri")
        sys.exit(1)

    with open("NEWS.yaml", "r", encoding="utf8") as stream:
        for news in yaml.safe_load_all(stream):
            short_version = version.Version(news['Version'])
            if short_version.major < 3:  # Don't create changelog for old versions not in Android
                continue
            if 'Description' in news:
                long_version = short_version.major*10000+short_version.minor*100+short_version.micro
                print(long_version, news['Description'])
                out_file = f"fastlane/metadata/android/en-US/changelogs/{long_version}.txt"
                with open(out_file, 'w', encoding="utf8") as changelog_file:
                    changelog_file.write('\n'.join(news['Description']) + '\n')


if __name__ == '__main__':
    main(sys.argv)
