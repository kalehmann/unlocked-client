## Unlocked client installation

### Arch Linux

First clone the repository with

```
git clone https://github.com/kalehmann/unlocked-client.git
```

Then create the package (as non root user) with

```
cd unlocked-client/packaging/aur
makepkg
```

Next install the package as root with

```
pacman -U <path-to-the-package>
```

After that configure the unlocked-client by editing
`/etc/unlocked/unlocked.conf` and finally enable the socket inside the
initramfs with

```
systemctl enable initrd-unlocked-client@unlocked.socket
```

