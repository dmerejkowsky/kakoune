set -x
set -e
this_dir=$(pwd)

(
  cd src
  make debug=no -j $(nproc)
)

(
  cd ~/.local/bin
  ln -sf ${this_dir}/src/kak
)
