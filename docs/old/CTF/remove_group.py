#!env python

import sys
import pwd
import grp
import os
import os.path

import subprocess

sys.argv.pop(0)
proj = sys.argv.pop(0)

gid_to_uname = {}
uname_to_home = {}

# Walk the password file and gather primary groups and home directories.  The
# GROUP under which the experiment is swapped in is all users primary group.
# Their membership does not appear in /etc/group so we have to gather those
# users here.
for pw in pwd.getpwall():
    if pw.pw_gid not in gid_to_uname:
	gid_to_uname[pw.pw_gid] = set()
    gid_to_uname[pw.pw_gid].add(pw.pw_name)
    uname_to_home[pw.pw_name] = pw.pw_dir

del_users = set()
del_groups = set()
# For each group get the members.  Save the groups for removing /groups
# filesystems.
for g in sys.argv:
    print "Removing group %s" % g
    gnam ="%s-%s" % (proj[0:3], g) 
    try:
     ginfo = grp.getgrnam(gnam)
     del_users |= set(ginfo.gr_mem)
     del_users |= gid_to_uname.get(ginfo.gr_gid, set())
     del_groups.add(g)
    except:
     pass

# Unmount each user's home dir and remove them.  Unmounting a home dir will
# fail if it is not mounted.  Deleting a user will fail if they are logged in.
# Being logged in includes running this as a startcmd.
for u in del_users:
    try:
	subprocess.check_call(['umount', uname_to_home[u]])
    except:
	pass
    try:
	subprocess.check_call(['deluser', u])
    except :
	print >>sys.stderr, 'Could not remove %s' % u

# Remove the group directories.  These are not always mounted, and umounts will
# fail in that case.
for g in del_groups:
    try:
	subprocess.check_call(['umount', os.path.join('/groups', proj, g)])
    except:
	pass
