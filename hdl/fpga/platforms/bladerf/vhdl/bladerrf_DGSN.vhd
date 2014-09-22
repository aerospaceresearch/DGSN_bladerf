library ieee ;
    use ieee.std_logic_1164.all ;
    use ieee.numeric_std.all ;

architecture dgsn_bladerf of bladerf is

    component baud_generator is
	   generic (
		  InputFreq  			: integer := 38400000 ;
		  OutputFreq 			: integer := 9600 
	   ) ;
		port ( 
		  c4_clock            :   in      std_logic ;
        uart_clk            :   out     std_logic := '0'
		) ;
	 end component baud_generator ;


begin

	 uart_baud_gen: baud_generator
		port map ( c4_clock => c4_clock, uart_clk => dac_sclk );
		
end architecture ; -- arch
