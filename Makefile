CC		= gcc
CFLAGS	= -Wextra -Wall -std=c11 -pedantic

make:
	${CC} ${CFLAGS} -O2 -o reminder reminder.c

debug:
	${CC} ${CFLAGS} -ggdb -o reminder_debug reminder.c

profile:
	${CC} ${CFLAGS} -pg -o reminder_profile reminder.c

clean:
	rm -f reminder
	rm -f reminder_debug
	rm -f reminder_profile
	
install: make
	mkdir -p /bin
	cp -f reminder /bin
	chmod 755 /bin/reminder

uninstall:
	rm -f /bin/reminder
