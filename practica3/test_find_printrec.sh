#!/usr/bin/expect -f
# 03
# find and printRec test. 
# Checks format of find, error messages
# and order of printRec

set timeout 1
set filename "test"
set programName "./library"

# delete all files starting with $filename
spawn rm -f $filename.db $filename.ind

# call program
spawn ./$programName first_fit $filename
expect "Type command and argument/s."
expect "exit"

# ADD BOOKS (Added out of order to check sorting in printRec)

# add first book (ID 12346)
send  "add 12346|978-2-12345681-3|La busca|Catedra\r"
expect "Record with BookID=12346 has been added to the database"
expect "exit"

# add second book (ID 12345 - Smaller ID)
send  "add 12345|978-2-12345680-3|El Quijote|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"

# add third book (ID 12347)
send  "add 12347|978-2-12345680-4|La colmena|Alfaguara\r"
expect "Record with BookID=12347 has been added to the database"
expect "exit"


# TEST FIND FUNCTION

# 1. Find existing record
send "find 12345\r"
# We expect exactly the format: ID|ISBN|Title|Editorial
expect "12345|978-2-12345680-3|El Quijote|Catedra"
expect "exit"

# 2. Find non-existing record
send "find 99999\r"
expect "Record with bookId=99999 does not exist"
expect "exit"


# TEST PRINTREC FUNCTION
# Should print records ordered by ID (Key) regardless of insertion order
send "printRec\r"
# First should be 12345
expect "12345|978-2-12345680-3|El Quijote|Catedra"
# Second should be 12346
expect "12346|978-2-12345681-3|La busca|Catedra"
# Third should be 12347
expect "12347|978-2-12345680-4|La colmena|Alfaguara"

expect "exit"
send "exit\r"

puts "1) Add, Find and PrintRec execution OK, ;-)"

if {[file exists [file join $filename.db]]} {
    puts "2) file $filename.db Exists, ;-)"
} else {
    puts "2) file $filename.db NOT found, :-("
}

puts "3) Script end"