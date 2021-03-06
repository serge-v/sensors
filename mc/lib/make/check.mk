check:
	@$(CC) $(ALL_CXXFLAGS) -E ../lib/make/check_pins.cpp | grep __RFM12B_PINS__ -A 100 | sed "/^$$/d" > this_build~.conf
	@if [ -f last_build~.conf ] ; then colordiff -u -L 'obj files compiled with:' -L 'new conflicting settings:' last_build~.conf this_build~.conf ; fi
	cat this_build~.conf

clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d) \
	last_build~.conf this_build~.conf
	ls -l --color=always


depend:
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(CXXSRC) $(ASRC) > $(TARGET).dep
