#!/bin/sh
#=============================================================================
# SPDX-FileCopyrightText: 2025 Johnny Jazeix <jazeix@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#=============================================================================

rootFolder=$(realpath $(dirname $0))
dataFolder=$(dirname $rootFolder)/share
$(dirname $0)/Li-ri $dataFolder/Li-ri/
