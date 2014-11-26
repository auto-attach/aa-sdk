How to Submit Patches to Auto Attach
====================================

Patches against the tip of the git master branch are preferred. Please, clone
the repository:

    git clone https://github.com/auto-attach/aa-sdk.git

Make any modification required. Commit the modifications with a meaningful
message:

 1. Use a descriptive first-line.

 2. Prepend the first line with the name of the subsystem modified
    (`apps`, `common`, `platform`, 'transport') or something a bit more
    precise.

 3. Don't be afraid to put a lot of details in the commit message.

Use `git format-patch` to generate patches to submit:

    git format-patch origin/master

