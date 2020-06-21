library ieee;
use ieee.std_logic_1164.all;

library work;
use work.wishbone_types.all;

entity toplevel is
    generic (
	MEMORY_SIZE   : positive := (384*1024);
	RAM_INIT_FILE : string   := "firmware.hex";
	RESET_LOW     : boolean  := true;
	CLK_INPUT     : positive := 100000000;
	CLK_FREQUENCY : positive := 100000000;
	USE_DRAM      : boolean  := false;
	DRAM_SIZE     : integer  := 0;
	DISABLE_FLATTEN_CORE : boolean := false;
        UART_IS_16550 : boolean  := true
	);
    port(
	ext_clk   : in  std_ulogic;
	ext_rst   : in  std_ulogic;

	-- UART0 signals:
	uart0_txd : out std_ulogic;
	uart0_rxd : in  std_ulogic;

	-- DRAM main data wishbone connection
	-- Remove the upper two address bits?
	signal wb_dram_in_adr : out std_ulogic_vector(28 downto 0);
	signal wb_dram_in_dat : out std_ulogic_vector(63 downto 0);
	signal wb_dram_in_cyc : out std_ulogic;
	signal wb_dram_in_stb : out std_ulogic;
	signal wb_dram_in_sel : out std_ulogic_vector(7 downto 0);
	signal wb_dram_in_we : out std_ulogic;
	signal wb_dram_out_dat : in std_ulogic_vector(63 downto 0)
		:= x"FFFFFFFFFFFFFFFF";
	signal wb_dram_out_ack : in std_ulogic := '0';
	signal wb_dram_out_stall : in std_ulogic := '0'
	);
end entity toplevel;

architecture behaviour of toplevel is

    -- Reset signals:
    signal soc_rst : std_ulogic;
    signal pll_rst : std_ulogic;

    -- Internal clock signals:
    signal system_clk : std_ulogic;
    signal system_clk_locked : std_ulogic;

    -- DRAM main data wishbone connection
    signal wb_dram_in       : wishbone_master_out;
    signal wb_dram_out      : wishbone_slave_out;

begin

    reset_controller: entity work.soc_reset
	generic map(
	    RESET_LOW => RESET_LOW
	    )
	port map(
	    ext_clk => ext_clk,
	    pll_clk => system_clk,
	    pll_locked_in => system_clk_locked,
	    ext_rst_in => ext_rst,
	    pll_rst_out => pll_rst,
	    rst_out => soc_rst
	    );

    clkgen: entity work.clock_generator
	generic map(
	    CLK_INPUT_HZ => CLK_INPUT,
	    CLK_OUTPUT_HZ => CLK_FREQUENCY
	    )
	port map(
	    ext_clk => ext_clk,
	    pll_rst_in => pll_rst,
	    pll_clk_out => system_clk,
	    pll_locked_out => system_clk_locked
	    );

    -- Main SoC
    soc0: entity work.soc
	generic map(
	    MEMORY_SIZE   => MEMORY_SIZE,
	    RAM_INIT_FILE => RAM_INIT_FILE,
	    SIM           => false,
	    CLK_FREQ      => CLK_FREQUENCY,
	    HAS_DRAM      => USE_DRAM,
	    DRAM_SIZE     => DRAM_SIZE,
	    DISABLE_FLATTEN_CORE => DISABLE_FLATTEN_CORE,
            UART0_IS_16550     => UART_IS_16550
	    )
	port map (
	    system_clk        => system_clk,
	    rst               => soc_rst,
	    uart0_txd         => uart0_txd,
	    uart0_rxd         => uart0_rxd,
	    wb_dram_in        => wb_dram_in,
	    wb_dram_out       => wb_dram_out
	    );


    has_dram: if USE_DRAM generate
    begin
        -- Make the address compatible with "generic" RAM:
        -- Cut off the segment prefix and narrow the address to word
        -- granularity.
        wb_dram_in_adr(28 downto 27) <= "00";
        wb_dram_in_adr(26 downto 0) <= wb_dram_in.adr(29 downto 3);
        wb_dram_in_dat <= wb_dram_in.dat;
        wb_dram_in_cyc <= wb_dram_in.cyc;
        wb_dram_in_stb <= wb_dram_in.stb;
        wb_dram_in_sel <= wb_dram_in.sel;
        wb_dram_in_we  <= wb_dram_in.we;
        wb_dram_out.ack <= wb_dram_out_ack;
        wb_dram_out.dat <= wb_dram_out_dat;
        wb_dram_out.stall <= wb_dram_out_stall;
    end generate;

end architecture behaviour;
