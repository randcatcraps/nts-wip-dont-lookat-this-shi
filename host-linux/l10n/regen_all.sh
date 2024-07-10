#!/bin/bash

tools=(xgettext msginit msgmerge)

for _tool in "${tools[@]}"; do
  echo -n "${_tool} is: "
  command -v "$_tool" || { echo gettext not installed, Stop. && exit 1; }
done

set -e

cd "$(dirname "$0")"

project_name=${project_name:-NtsHostLinux}
pot_file="${project_name}.pot"
locales=(ja zh_CN)

echo '==> regenerating pot..'
find .. -type f -name \*.c | xargs xgettext -L C -j -c -k_ \
  -o "$pot_file" --omit-header --no-location

echo '==> updating po files..'
for _locale in "${locales[@]}"; do
  _po_file="${_locale}.po"
  if [ -e "$_po_file" ]; then
    (set -x && msgmerge -U "$_po_file" "$pot_file")
  else
    echo "!!! locale '${_locale}' doesn't exist yet, creating.."
    (set -x && msginit -l "${_locale}.UTF-8" -i "$pot_file" \
      -o "$_po_file" --no-translator)
  fi
done
