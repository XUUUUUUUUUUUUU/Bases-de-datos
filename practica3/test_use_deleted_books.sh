#!/usr/bin/expect -f
# 02
# add test. Does not check 
# the index or the reuse of deleted registers
# first test
set timeout 1
set filename "test"
set programName "./library"

# delete all files starting with $filename
spawn rm -f $filename.db $filename.ind

# call program
spawn ./$programName first_fit $filename
expect "Type command and argument/s."
expect "exit"

# ADD BOOKS
# add first book
send  "add 12346|978-2-12345680-3|El Quijote|Catedra\r"
expect "Record with BookID=12346 has been added to the database"
expect "exit"
# add second book
send  "add 12345|978-2-12345086-3|La busca|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"

# add third book
send  "add 12347|978-2-12345680-4|el quijote|catedra\r"
expect "Record with BookID=12347 has been added to the database"
expect "exit"

# add fourth book
send  "add 12348|978-2-12345086-3|la busca|catedra\r"
expect "Record with BookID=12348 has been added to the database"
expect "exit"

# check index
# print index
puts "------------------------"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12347"
expect "    offset: #90"
expect "    size: #38"
expect "Entry #3"
expect "    key: #12348"
expect "    offset: #136"
expect "    size: #36"
expect "exit"

# delete second books
send "del 12345\r"
expect "Record with BookID=12347 has been deleted"
expect "exit"
send "printInd\n"
expect "    key: #12346"
expect "    key: #12347"
expect "    key: #12348"
expect "exit"

# add a book with the same size or less size as seconde books
send "add 99999|978-0-00000000-0|Reuse|Yes\r"
expect "Record with BookID=99999 has been added"
send "printInd\r"
expect {
    "key: #99999\r\n    offset: #46" {
        send_user "\n\nSUCCESS: Space Reuse Verified! New book is at offset 46.\n"
    }
    "key: #99999\r\n    offset: #136" { 
        send_user "\n\nFAILURE: New book appended to end, hole NOT reused.\n"
        exit 1
    }
    timeout {
        send_user "\n\nFAILURE: Test timed out.\n"
        exit 1
    }
}

send "exit\r"
expect eof