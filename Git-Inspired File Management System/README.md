<h1>METHOD TO RUN PROGRAM:</h1>

FOR VSCODE:
1. Save "run.sh", ""code.cpp", "linkedlist.h", "heap.h", "mystack.h", "README.md" and "hashmap.h" in same folder.
2. Run "run.sh".

FOR TERMINAL RUNNING:
1. go to the folder containing all the above given files.
2. chmod +x run.sh
3. ./run.sh


<h1>AVAILABLE COMMANDS:</h1>
CREATE <filename>
Creates a new file with version 0 (empty content) and marks it as a snapshot with an initial message like “init”.

READ <filename>
Prints content of the file’s active version. If the file doesn’t exist, prints an error.

INSERT <filename> <content>
Appends content to the active version if it is not a snapshot. If the active version is already a snapshot, creates a new child version that copies content, appends the text, and becomes the new active version.

UPDATE <filename> <content>
Replaces the content similarly to INSERT (edit in place if mutable; otherwise fork a new version).

SNAPSHOT <filename> <message>
Marks the active version as immutable, stores message, and records snapshot time.

ROLLBACK <filename> [versionID]
If versionID is provided: moves the active pointer to that version (must exist in the version tree).
If omitted: moves active to the parent of the current active version (if it exists).

HISTORY <filename>
Lists all ancestor versions of current version in chronological order with version_id, timestamp, and message. ("version_id timestamp message")

RECENTFILES [num]
Lists [num] most recently modified files.

BIGGESTTREES [num]
Lists top [num] files sorted by total version count (largest first).

EXIT
Exits from the program

<h1>Precaution:</h1>
1. Do not use spaces while naming files.<br>
2. System handles only one command at a time.<br>
3. The timestamp provided during implementation of HISTORY function is expressed in terms of number of seconds since Jan 1, 1970 UTC.
