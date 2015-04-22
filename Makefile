
CC=arm-none-linux-gnueabi-
LIBS += -lpthread -lrt -lm

target: bat_main.o osal.o volt_meas.o lcd_disp.o gprs.o
	$(CC)gcc -O3 $(LIBS) -o man_bat $^

bat_main.o: bat_main.c osal.h volt_meas.h lcd_disp.h gprs.h  
	$(CC)gcc -O3 -c $<
osal.o: osal.c osal.h
	$(CC)gcc -O3 -c $<
volt_meas.o: volt_meas.c osal.h
	$(CC)gcc -O3 -c $<
lcd_disp.o: lcd_disp.c osal.h
	$(CC)gcc -O3 -c $<
gprs.o: gprs.c osal.h
	$(CC)gcc -O3 -c $<
#parse_config.o: parse_config.c
#	$(CC)gcc -O3 -c $<

clean:
	@rm -vf ger
	@rm -vf *.o
