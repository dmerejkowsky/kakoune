set -x
set -e
this_dir=$(pwd)

(
  cd src
  make debug=no -j $(nproc)
)

(
  mkdir -p ~/.config/kak
  cd ~/.config/kak/
  ln -sf ${this_dir}/kakrc

  cd ~/.local/bin
  ln -sf ${this_dir}/src/kak
)
