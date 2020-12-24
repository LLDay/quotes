FROM archlinux

RUN pacman -Sy && pacman -S cmake make gcc protobuf boost --noconfirm
