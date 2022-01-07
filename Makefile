OUT = fryzjerzy
CC = gcc
ODIR = obj
SDIR = src

_OBJS := fryzjerzy.o
_OBJS += fryzjerzy_cash_machine.o
_OBJS += fryzjerzy_client.o
_OBJS += fryzjerzy_logger.o
_OBJS += fryzjerzy_money.o
_OBJS += fryzjerzy_semaphores_helpers.o
_OBJS += fryzjerzy_waiting_room.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(OUT): $(OBJS) 
	$(CC) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c 
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)