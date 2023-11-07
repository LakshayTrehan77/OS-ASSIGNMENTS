Question 1 : Running the ARTIX LINUX using Virtual Machine

In the first question of the assignment, we had to set up a testbench in Virtual Machine. We had installed Artix-base (runit-version) from the official website.

For the smooth functioning and to avoid memory/space issues, we have allocated RAM of more than 4 GB. With that, we also assigned approximately 2 or more virtual CPUs and a hard drive space of not less than 20 GB.

We installed the latest available runit version available from the official website of Artix and then followed the set of instructions provided on the site for installation.

We made sure that some of the post installation steps were completed including to enable ArchLinux repositories, enabling EFI, etc.

We have also installed the following packages:
    1. binutils
    2. elfutils
    3. gcc
    4. gdb
    5. make
    6. automake
    7. autoconf
    8. yasm
    9. vim

For the Kernel compilation part, we unpacked the tarball and run a 'make mrproper' to clean up build artifacts. Then we renamed the 'config-rev-9-gold' file as '.config' after copying it to the unpacked directory. Then the kernel was compiled.