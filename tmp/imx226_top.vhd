-- ------------------------------------------------------------------------------------
--                 file : imx226_top.vhd
--        creation date : 23-06-2017 15:38
--           created by : Hubert Li
--        last modified : 2017年06月26日 20:57:19
--                                                                               
--            copyright : 2015-2019 by Hubert Li
--                             Strictly Confidential
--                             All rights reserved.
--                       No part of this hardware description, either
--                       material or conceptual may be copied or distributed,
--                       transmitted, transcribed, stored in a retrieval system
--                       or translated into any human or computer language in
--                       any form by any means, electronic, mechanical, manual
--                       or otherwise, or disclosed to third parties without
--                       the express written permission of Hubert Li
--                                                                               
-- ------------------------------------------------------------------------------------

library ieee;

use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

-- pragma translate_off
-- pragma coverage off
library std;
use std.textio.all;
use ieee.std_logic_textio.all;
use work.msf_fileio_pack.all;
-- pragma coverage on
-- pragma translate_on


entity imx226_top is
  port (
    clk                                           : in  std_logic;
    resetn                                        : in  std_logic;
    mos_clk                                       : in  std_logic;
    mos_resetn                                    : in  std_logic;
    ddr_clk                                       : in  std_logic;
    ddr_resetn                                    : in  std_logic;
    ----------------------------
    -- Sensor
    ----------------------------
    imx226_xclr                                   : out std_logic;
    imx226_xce                                    : out std_logic;
    imx226_sck                                    : out std_logic;
    imx226_sdi                                    : out std_logic;
    imx226_xvs                                    : out std_logic;
    imx226_xhs                                    : out std_logic;
    -- -------------------------
    -- serial debug
    -- -------------------------
    us_dbg_tx_port                                : out std_logic;
    us_dbg_rx_port                                : in  std_logic;
    -- -------------------------
    -- serial sys
    -- -------------------------
    us_sys_tx_port                                : out std_logic;
    us_sys_rx_port                                : in  std_logic;
    ---------------------------
    -- raw 
    ---------------------------
    raw_in_valid                                  : in  std_logic;
    raw_in_enable                                 : in  std_logic;
    raw_in_pixel                                  : in  std_logic_vector(199 downto 0);
    ---------------------------
    -- ddr
    ---------------------------
    ddr_req_valid                                 : out std_logic;
    ddr_req_enable                                : in  std_logic;
    ddr_req_rnw                                   : out std_logic;
    ddr_req_addr                                  : out std_logic_vector(23 downto 0);
    ddr_req_data                                  : out std_logic_vector(255 downto 0);
    ddr_rtn_valid                                 : in  std_logic;
    ddr_rtn_data                                  : in  std_logic_vector(255 downto 0)
  );
end entity imx226_top;



architecture rtl of imx226_top is

  ---------------------------------
  -- component
  ---------------------------------
  -- {{{
  component msf_cdc_sync
    generic (
      w                                           : integer  := 4
    );
    port (
      wt_clk                                      : in  std_logic;
      rd_clk                                      : in  std_logic;
      wt_data                                     : in  std_logic_vector(w-1 downto 0);
      rd_data                                     : out std_logic_vector(w-1 downto 0)
    );
  end component msf_cdc_sync;

  component imx226_console_uart 
     generic (
        addr_dw                                   : integer   := 3;
        mos_dw                                    : integer   := 1;
        reg_dw                                    : integer   := 4;
        mem_dw                                    : integer   := 32
     );
    port (
      -- only for 162M
      clk                                         : in  std_logic;
      resetn                                      : in  std_logic;
      -- -------------------------
      -- serial debug
      -- -------------------------
      us_dbg_tx_port                              : out std_logic;
      us_dbg_rx_port                              : in  std_logic;
      -- -------------------------
      -- serial sys
      -- -------------------------
      us_sys_tx_port                              : out std_logic;
      us_sys_rx_port                              : in  std_logic;
      -- -------------------------
      -- isp_csl_req, always write to cmos
      -- -------------------------
      isp_csl_req_valid                           : in  std_logic;
      isp_csl_req_enable                          : out std_logic;
      isp_csl_req_addr                            : in  std_logic_vector(addr_dw*8-1 downto 0);
      isp_csl_req_data                            : in  std_logic_vector(mos_dw*8-1 downto 0);
      -- -------------------------
      -- serial interface
      -- -------------------------
      xce                                         : out std_logic;
      sck                                         : out std_logic;
      sdi                                         : out std_logic;
      -- -------------------------
      -- regbank_interface
      -- -------------------------
      regbank_req_valid                           : out std_logic;
      regbank_req_enable                          : in  std_logic;
      regbank_req_rnw                             : out std_logic;
      regbank_req_addr                            : out std_logic_vector(addr_dw*8-1 downto 0);
      regbank_req_data                            : out std_logic_vector(reg_dw*8-1 downto 0);
      regbank_rtn_valid                           : in  std_logic;
      regbank_rtn_enable                          : out std_logic;
      regbank_rtn_data                            : in  std_logic_vector(reg_dw*8-1 downto 0);
      -- -------------------------
      -- mem
      -- -------------------------
      mem_req_valid                               : out std_logic;
      mem_req_enable                              : in  std_logic;
      mem_req_rnw                                 : out std_logic;
      mem_req_addr                                : out std_logic_vector(addr_dw*8-1 downto 0);
      mem_req_data                                : out std_logic_vector(mem_dw*8-1 downto 0);
      mem_rtn_valid                               : in  std_logic;
      mem_rtn_enable                              : out std_logic;
      mem_rtn_data                                : in  std_logic_vector(mem_dw*8-1 downto 0);
      -- -------------------------
      -- ram0
      -- -------------------------
      ram0_req_valid                            : out std_logic;
      ram0_req_enable                           : in  std_logic;
      ram0_req_rnw                              : out std_logic;
      ram0_req_addr                             : out std_logic_vector(addr_dw*8-1 downto 0);
      ram0_req_data                             : out std_logic_vector(ram0_dw*8-1 downto 0);
      ram0_rtn_valid                            : in  std_logic;
      ram0_rtn_enable                           : out std_logic;
      ram0_rtn_data                             : in  std_logic_vector(ram0_dw*8-1 downto 0)
     );
  end component imx226_console_uart;

  -- }}}
  ------------------------------
  -- self define type 
  ------------------------------
  TYPE imx226_reg_table         is array(128 downto 0) of std_logic_vector(39 downto 0); 

  ---------------------------------
  -- signal  
  ---------------------------------
  -- {{{
  -- xhs
  signal xhs_buf_wt_valid                         : std_logic;
  signal xhs_buf_wt_enable                        : std_logic;
  signal xhs_buf_rd_valid                         : std_logic;
  signal xhs_buf_rd_enable                        : std_logic;
  signal xhs_buf_rd_last                          : std_logic;
  signal xhs_buf_h_cnt                            : std_logic_vector(11 downto 0);
  signal xhs_buf_start_pos                        : std_logic;
  signal xhs_buf_end_neg                          : std_logic;
  signal xhs_buf_data                             : std_logic;
  signal hs_inck_count                            : std_logic_vector(11 downto 0);

  -- xvs
  signal xvs_buf_wt_valid                         : std_logic;
  signal xvs_buf_wt_enable                        : std_logic;
  signal xvs_buf_rd_valid                         : std_logic;
  signal xvs_buf_rd_enable                        : std_logic;
  signal xvs_buf_v_cnt                            : std_logic_vector(11 downto 0);
  signal xvs_buf_start_pos                        : std_logic;
  signal xvs_buf_end_neg                          : std_logic;
  signal xvs_buf_data                             : std_logic;
  signal vs_line_count                            : std_logic_vector(11 downto 0);

  -- cmos ms
  signal cmos_ms_cnt                              : std_logic_vector(13 downto 0);
  signal cmos_ms_rd_valid                         : std_logic;
  signal cmos_ms_rd_trigger                       : std_logic;

  -- cmos delay
  signal cmos_init_start_delay                    : std_logic_vector( 2 downto 0);

  -- cmos_init                          
  signal imx226_table                             : imx226_reg_table  := (
  );
  signal cmos_init_wt_valid                       : std_logic;
  signal cmos_init_wt_enable                      : std_logic;
  signal cmos_init_rd_valid                       : std_logic;
  signal cmos_init_rd_enable                      : std_logic;
  signal cmos_init_rd_last                        : std_logic;
  signal cmos_init_addr                           : std_logic_vector(23 downto 0);
  signal cmos_init_data                           : std_logic_vector( 7 downto 0);
  signal cmos_init_delay                          : std_logic_vector( 7 downto 0);
  signal cmos_init_reg_cnt                        : std_logic_vector( 7 downto 0);
  signal cmos_init_data_valid                     : std_logic;

  -- console uart 
  signal isp_cs_reg_valid                         : std_logic;
  signal isp_cs_reg_enable                        : std_logic;
  signal isp_csl_req_addr                         : std_logic_vector(23 downto 0);
  signal isp_csl_req_data                         : std_logic_vector( 7 downto 0);
  -- }}}

begin

  imx226_xclr       <= mos_resetn;
  imx226_xhs        <= xhs_buf_data;
  imx226_xvs        <= xvs_buf_data;
  
  ------------------------------------
  -- mode4
  -- xvs: min = 2199
  -- xhs: min = 4248 pixel = 2165 DCK = 1083 lvds_clk = 542 mos_clk
  ---------------------------------
  -- xhs 
  ---------------------------------
  -- {{{

  msf_cdc_sync_inst_hs_inck: msf_cdc_sync
    generic map (
      w                                           => 1 
    )
    port map (
      wt_clk                                      => clk, 
      rd_clk                                      => mos_clk, 
      wt_data                                     => gr_hs_inck_count, 
      rd_data                                     => hs_inck_count, 
    );
 
  xhs_buf_wt_valid      <= '1';

  -- **Sequential Process
  -- Purpose:
  p_xhs_buf_rd_valid: process(mos_clk,mos_resetn)
  begin 
    if (mos_resetn = '0') then
      xhs_buf_rd_valid      <= '0';
    elsif (mos_clk'event and mos_clk = '1') then
      if (xhs_buf_wt_valid = '1') and (xhs_buf_wt_enable = '1') then
        xhs_buf_rd_valid      <= '1';
      elsif (xhs_buf_rd_valid = '1') and (xhs_buf_rd_enable = '1') and (xhs_buf_rd_last = '1') then
        xhs_buf_rd_valid      <= '0';
      end if;
    end if;
  end process p_xhs_buf_rd_valid;

  xhs_buf_wt_enable     <= (xhs_buf_rd_enable and xhs_buf_rd_last) or not xhs_buf_rd_valid;

  -- **Sequential Process
  -- Purpose:
  p_xhs_buf_h_cnt: process(mos_clk,mos_resetn)
  begin
    if (mos_resetn = '0') then
      xhs_buf_h_cnt         <= (others => '0');
    elsif (mos_clk'event and mos_clk = '1') then
      if (xhs_buf_wt_valid = '1') and (xhs_buf_wt_enable = '1') then
        xhs_buf_h_cnt         <= unsigned(hs_inck_count) - '1';
      elsif (xhs_buf_rd_valid = '1') and (xhs_buf_rd_enable = '1') then
        xhs_buf_h_cnt         <= unsigned(xhs_buf_h_cnt) - '1';
      end if;
    end if;
  end process p_xhs_buf_h_cnt;
  
  xhs_buf_rd_last         <= not or_reduce(xhs_buf_h_cnt);

  xhs_buf_start_pos       <= '1' when unsigned(xhs_buf_h_cnt) = (unsigned(hs_inck_count) -7) else
                             '0';
  xhs_buf_end_neg         <= xhs_buf_rd_last;

  -- **Sequential Process
  -- Purpose:
  p_xhs_buf_data: process(mos_clk,mos_resetn)
  begin
    if (mos_resetn = '0') then
      xhs_buf_data    <= '0';
    elsif(mos_clk'event and mos_clk = '1') then
      if (xhs_buf_wt_valid = '1') and (xhs_buf_wt_enable = '1') then
        xhs_buf_data    <= '0';
      elsif (xhs_buf_rd_valid = '1') and (xhs_buf_rd_enable = '1') then
        if (xhs_buf_start_pos = '1')
          xhs_buf_data    <= '1';
        elsif (xhs_buf_end_neg = '1') then
          xhs_buf_data    <= '0';
        end if;
      end if;
    end if;
  end process p_xhs_buf_data;
  
  xhs_buf_rd_enable       <= xvs_buf_wt_enable;
  
  -- }}}

  ---------------------------------
  -- xvs 
  ---------------------------------
  -- {{{
  msf_cdc_sync_inst_vs_line: msf_cdc_sync
    generic map (
      w                                           => 1 
    )
    port map (
      wt_clk                                      => clk, 
      rd_clk                                      => mos_clk, 
      wt_data                                     => gr_vs_line_count, 
      rd_data                                     => vs_line_count, 
    );
 
  xvs_buf_wt_valid        <= xhs_buf_rd_valid;
  -- **Sequential Process
  -- Purpose:
  p_xvs_buf_rd_valid: process(mos_clk,mos_resetn)
  begin 
    if (mos_resetn = '0') then
      xvs_buf_rd_valid      <= '0';
    elsif (mos_clk'event and mos_clk = '1') then
      if (xvs_buf_wt_valid = '1') and (xvs_buf_wt_enable = '1') then
        xvs_buf_rd_valid      <= '1';
      elsif (xvs_buf_rd_valid = '1') and (xvs_buf_rd_enable = '1') then
        xvs_buf_rd_valid      <= '0';
      end if;
    end if;
  end process p_xvs_buf_rd_valid;

  xvs_buf_wt_enable     <= xvs_buf_rd_enable or not xvs_buf_rd_valid;

  -- **Sequential Process
  -- Purpose:
  p_xvs_buf_v_cnt: process(mos_clk,mos_resetn)
  begin
    if (mos_resetn = '0') then
      xvs_buf_v_cnt         <= (others => '0');
    elsif (mos_clk'event and mos_clk = '1') then
      if (xvs_buf_wt_valid = '1') and (xvs_buf_wt_enable = '1') and (xhs_buf_start_pos = '1')then
        if (or_reduce(xvs_buf_v_cnt) = '0') then
          xvs_buf_v_cnt       <= unsigned(vs_line_cnt) - '1';
        else
          xvs_buf_v_cnt       <= unsigned(xvs_buf_v_cnt) - '1';
        end if;
      end if;
    end if;
  end process p_xvs_buf_v_cnt;

  xvs_buf_start_pos     <= '1' when (unsigned(xvs_buf_v_cnt) = 2) and xhs_buf_start_pos else
                           '0';
  xvs_buf_end_neg       <= '1' when (unsigned(xvs_buf_v_cnt) = 0) and xhs_buf_end_neg else
                           '0';

  -- **Sequential Process
  -- Purpose:
  p_xvs_buf_data: process(mos_clk,mos_resetn)
  begin
    if (mos_resetn = '0') then
      xvs_buf_data      <= '0';
    elsif (mos_clk'event and mos_clk = '1') then
      if (xvs_buf_wt_valid = '1') and (xvs_buf_wt_valid = '1') then
        if (xvs_buf_start_pos = '1') then
          xvs_buf_data      <= '1';
        elsif (xvs_buf_end_pos = '1') then
          xvs_buf_data      <= '0';
        end if;
      end if;
    end if;
  end process p_xvs_buf_data;

  xvs_buf_rd_enable       <= '1';
  -- }}}

  ---------------------------------
  -- cmos init
  ---------------------------------
  -- {{{

  -- **Sequential Process
  -- Purpose:
  p_cmos_ms_cnt: process(clk,resetn)
  begin
    if (resetn = '0') then
      cmos_ms_cnt       <= (others => '0');
    elsif (clk'event and clk = '1') then
      if (or_reduce(cmos_ms_cnt) = '0') then
        cmos_ms_cnt       <= conv_unsigned(162000,14);
      else
        cmos_ms_cnt       <= unsigned(cmos_ms_cnt) - '1';
      end if;
    end if;
  end process p_cmos_ms_cnt;

  cmos_ms_rd_valid        <= '1';
  cmos_ms_rd_trigger      <= not or_reduce(cmos_ms_cnt);
                    
  -- **Sequential Process
  -- Purpose:
  p_cmos_init_start_delay: process(clk,resetn)
  begin
    if (resetn = '0') then
      cmos_init_start_delay     <= conv_unsigned(5,3);
    elsif (clk'event and clk = '1') then
      if (cmos_ms_rd_trigger = '1') then
        if (or_reduce(cmos_init_start_delay) = '1') then
          cmos_init_start_delay     <= unsigned(cmos_init_start_delay) - '1';
        end if;
      end if;
    end if;
  end process p_cmos_init_start_delay;
  
  cmos_init_wt_valid      <= not or_reduce(cmos_init_start_delay) when (and_reduce(cmos_init_reg_cnt) = 0) else
                             '0';

  -- **Sequential Process
  -- Purpose:
  p_cmos_init_rd_valid: process(clk,resetn)
  begin
    if (resetn = '0') then
      cmos_init_rd_valid        <= '0';
    elsif (clk'event and clk = '1') then
      if (cmos_init_wt_valid = '1') and (cmos_init_wt_enable = '1') then
        cmos_init_rd_valid        <= '1';
      elsif (cmos_init_rd_valid = '1') and (cmos_init_rd_enable = '1') and (cmos_init_rd_last = '1') then
        cmos_init_rd_valid        <= '0';
      end if;
    end if;
  end process p_cmos_init_rd_valid;
  
  cmos_init_wt_enable     <= (cmos_init_rd_enable and cmos_init_rd_last) or not cmos_init_rd_valid;
  
  cmos_init_rd_last       <= not or_reduce(cmos_init_delay);

  -- **Sequential Process
  -- Purpose:
  p_cmos_init_main: process(clk,resetn)
  begin
    if (resetn = '0') then
      cmos_init_addr        <= (others => '0');
      cmos_init_data        <= (others => '0');
      cmos_init_delay       <= (others => '0');
      cmos_init_data_valid  <= '0';
      cmos_init_reg_cnt     <= (others => '0');
    elsif (clk'event and clk = '1') then
      if (cmos_init_wt_valid = '1') and (cmos_init_wt_enable = '1') then
        cmos_init_reg_cnt     <= unsigned(cmos_init_reg_cnt) + '1';
        cmos_init_addr        <= imx226_table(conv_integer(cmos_init_reg_cnt))(39 downto 16);
        cmos_init_data        <= imx226_table(conv_integer(cmos_init_reg_cnt))(15 downto  8);
        cmos_init_delay       <= imx226_table(conv_integer(cmos_init_reg_cnt))( 7 downto  0);
        cmos_init_data_valid  <= '1';
      elsif (cmos_init_rd_valid  = '1') and (cmos_init_rd_enable = '1') then
        cmos_init_data_valid  <= '0';
        if (cmos_ms_rd_trigger = '1') and (or_reduce(cmos_init_delay) = '1') then
          cmos_init_delay       <= unsigned(cmos_init_delay) - '1';
        end if;
      end if;
    end if;
  end process p_cmos_init_main;


  cmos_init_rd_enable         <= isp_csl_req_enable when cmos_init_data_valid = '1' else
                                 '1';
  -- }}}

  ---------------------------------
  -- console uart 
  ---------------------------------
  -- {{{
  isp_csl_req_valid           <= cmos_init_rd_valid and cmos_init_data_valid;
  isp_csl_req_addr            <= cmos_init_addr;
  isp_csl_req_data            <= cmos_init_data;

  imx226_console_uart_inst: imx226_console_uart 
     generic map (
        addr_dw                                   => 3,
        mos_dw                                    => 1,
        reg_dw                                    => 4,
        mem_dw                                    => 32
     )
    port map (
      -- only for 162M
      clk                                         => clk, 
      resetn                                      => resetn,
      -- -------------------------
      -- serial debug
      -- -------------------------
      us_dbg_tx_port                              => us_dbg_tx_port, 
      us_dbg_rx_port                              => us_dbg_rx_port,
      -- -------------------------
      -- serial sys
      -- -------------------------
      us_sys_tx_port                              => us_sys_tx_port, 
      us_sys_rx_port                              => us_sys_rx_port, 
      -- -------------------------
      -- isp_csl_req, always write to cmos
      -- -------------------------
      isp_csl_req_valid                           => isp_csl_req_valid, 
      isp_csl_req_enable                          => isp_csl_req_enable, 
      isp_csl_req_addr                            => isp_csl_req_addr, 
      isp_csl_req_data                            => isp_csl_req_data, 
      -- -------------------------
      -- serial interface
      -- -------------------------
      xce                                         => imx226_xce, 
      sck                                         => imx226_sck, 
      sdi                                         => imx226_sdi, 
      -- -------------------------
      -- regbank_interface
      -- -------------------------
      regbank_req_valid                           : out std_logic;
      regbank_req_enable                          : in  std_logic;
      regbank_req_rnw                             : out std_logic;
      regbank_req_addr                            : out std_logic_vector(addr_dw*8-1 downto 0);
      regbank_req_data                            : out std_logic_vector(reg_dw*8-1 downto 0);
      regbank_rtn_valid                           : in  std_logic;
      regbank_rtn_enable                          : out std_logic;
      regbank_rtn_data                            : in  std_logic_vector(reg_dw*8-1 downto 0);
      -- -------------------------
      -- mem
      -- -------------------------
      mem_req_valid                               : out std_logic;
      mem_req_enable                              : in  std_logic;
      mem_req_rnw                                 : out std_logic;
      mem_req_addr                                : out std_logic_vector(addr_dw*8-1 downto 0);
      mem_req_data                                : out std_logic_vector(mem_dw*8-1 downto 0);
      mem_rtn_valid                               : in  std_logic;
      mem_rtn_enable                              : out std_logic;
      mem_rtn_data                                : in  std_logic_vector(mem_dw*8-1 downto 0);
      -- -------------------------
      -- ram0
      -- -------------------------
      ram0_req_valid                            : out std_logic;
      ram0_req_enable                           : in  std_logic;
      ram0_req_rnw                              : out std_logic;
      ram0_req_addr                             : out std_logic_vector(addr_dw*8-1 downto 0);
      ram0_req_data                             : out std_logic_vector(ram0_dw*8-1 downto 0);
      ram0_rtn_valid                            : in  std_logic;
      ram0_rtn_enable                           : out std_logic;
      ram0_rtn_data                             : in  std_logic_vector(ram0_dw*8-1 downto 0)
     );
  end component imx226_console_uart;

  -- }}}

  raw_in_enable             <= raw_hloc_wt_valid;
  ---------------------------------
  -- raw hloc
  ---------------------------------
  -- {{{
  raw_hloc_wt_valid        <= raw_in_valid;

  -- **Sequential Process
  -- Purpose:
  p_raw_hloc_rd_valid: process(clk,resetn)
  begin
    if (resetn = '0') then
      raw_hloc_rd_valid      <= '0';
    elsif (clk'event and clk = '1') then
      if (raw_hloc_wt_valid = '1') and (raw_hloc_wt_enable = '1') and (raw_hloc_wt_last = '1') then
        raw_hloc_rd_valid      <= '1';
      elsif (raw_hloc_rd_valid = '1') and (raw_hloc_rd_enable = '1') and (raw_hloc_rd_last = '1') then
        raw_hloc_rd_valid      <= '0';
      end if;
    end if;
  end process p_raw_hloc_rd_valid;
  
  raw_hloc_wt_enable     <= (raw_hloc_rd_enable and raw_hlock_rd_last) or not raw_hloc_rd_valid;

  -- }}}

end architecture rtl;
