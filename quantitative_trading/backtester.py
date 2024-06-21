import datetime
import pandas as pd
import numpy as np
from datetime import datetime
from dateutil.relativedelta import relativedelta as delta

class Position():
    '''
    Position class. Holds name, date, entry cash, price, and amount of position.\n
    Requires a given type (short or long) to initialize
    '''
    def __init__(self, ptype):
        if ptype not in ['long', 'short']:
            raise ValueError(f'Invalid type: {ptype}')
        
        self.type = 1
        if ptype == 'short':
            self.type = -1

        self.name = ''
        self.date = ''
        self.entry_cash = np.nan
        self.price = np.nan
        self.amount = np.nan

    def open(self, name: str, date: str,
             entry_cash: float, price: float, 
             supress_print = False):
        '''
        Opens a new position given name of ETF, date of trade, entry cash for trade, and price of ETF.\n
        Only accepts 'kxi' or 'xlp' as ETF names.\n
        supress_print decides if print statements should be suppressed. \n
        Determines number of shares given entry cash and price of ETF.\n
        Raises AssertionError if given position is already open.
        '''
        
        if self.is_open():
            raise AssertionError(f'Position is already open. Close it before opening a new one.\
                                 \n Details: {self.get_position()}')
        if not supress_print:
            print(f'Opening position of type: {self.type}')
        typename = 'long' 
        if self.type == -1:
            typename = 'short'

        if name not in ['kxi', 'xlp']:
            raise ValueError(f"Invalid ETF name: {name}\n Name should be one of 'kxi', 'xlp'.")

        # get amount 
        amt = round(entry_cash/price)

        self.name = name
        self.date = date # save date of trade
        self.entry_cash = entry_cash # entry cash
        self.price = price
        self.amount = amt
        
        if not supress_print:
            print(f'Opened {typename} position with ${entry_cash}: \n\
                     {amt} shares of {name} at ${price}')

    def close(self, price, supress_print = False):
        '''
        Closes given position given price of ETF.\n
        supress_print decides if print statements should be suppressed. \n
        Returns pnl of position.\n
        Raises AssertionError if given position is not open.
        '''
        if not self.is_open():
            raise AssertionError('Position is already closed. Open before trying to close.')
        if not supress_print:
            print('Closing position...')
        pnl = self.get_pnl(price, supress_print)

        self.name = ''
        self.date = ''
        self.entry_cash = np.nan
        self.price = np.nan
        self.amount = np.nan

        if not supress_print:
            print(f'Closed position -- {pnl=}')
        return pnl
    
    def get_pnl(self, price, supress_print = False):
        '''
        Given current price, returns pnl of position.\n
        supress_print decides if print statements should be suppressed.
        '''
        typename = 'long'
        if self.type == -1:
            typename = 'short'
        # get initial value
        initial_val = self.type * self.entry_cash
        # calculate current value based on today's price and 
        # position's shares.
        # Note: self.type is 1 if position type is long
        new_mtm = self.type * self.amount * price
        # save new pnl
        pnl = new_mtm - initial_val
        if not supress_print:
            print(f'Theoretical pnl for {typename} position on {self.name} is {pnl}')
        return pnl
        
    def is_open(self, supress_print = True):
        '''
        Determines if given position is open or not.\n
        supress_print decides if print statements should be suppressed.
        '''
        
        if self.name:
            if not supress_print:
                print('Currently holding a position. Use get_position() for details.')
            return True
        else:
            return False
        
    def get_position(self):
        '''
        Returns dict of all attributes of position.
        '''
        return {'name': self.name, 
                'type': self.type,
                'date': self.date, 
                'price': self.price,
                'amount': self.amount,
                'entry_cash': self.entry_cash}

    def get(self, attr, supress_print = False):
        '''
        Returns given attribute of position.\n
        supress_print determines if print statements should be suppressed. 
        '''
        if not supress_print:
            print(f'{attr}={self.__getattribute__(attr)}')
        return self.__getattribute__(attr)
    
def find_short_long(short_pos: Position, long_pos: Position, 
                kxi_price: float, xlp_price: float):
    '''
    Given short and long positions, and current price of kxi and xlp,\n
    determines which is the short_price and which is the long_price, and \n
    returns the answer in a list as [short_price, long_price].
    '''
     
    short_name = short_pos.name
    #long_name = long_pos.name
    if short_name == 'kxi':
        short_price = kxi_price 
        long_price = xlp_price
    else:
        short_price = xlp_price
        long_price = kxi_price

    return [short_price, long_price]

def get_pnls(short_pos: Position, long_pos: Position, 
             kxi_price: float, xlp_price: float,
             zeta: float, s: float | None, 
             supress_print: bool):
    '''
    Given short and long positions, prices of kxi and xlp, zeta (trading cost), \n
    and s, stop loss condition limit, returns list [Bool, float], where \n
    Bool is the answer to whether stop loss condition was met, and float is the total pnl.
    supress_print determines if print statements should be supressed.\n
    Raises AssertionError if either of the positions are not open.
    '''
    
    if (not short_pos.is_open()) or (not long_pos.is_open()):
        raise AssertionError(f'Not all positions are open. Cannot get pnl:\n\
                             short_pos: {short_pos.get_position()}\n\
                             long_post: {long_pos.get_position()}'
                             )
    
    prices = find_short_long(short_pos, long_pos, kxi_price, xlp_price)
    
    short_price = prices[0]
    long_price = prices[1]

    pnl_short = short_pos.get_pnl(short_price, supress_print)
    pnl_long = long_pos.get_pnl(long_price, supress_print)

    # get total position pnl -- incorporating trading cost, zeta
    pnl = (pnl_short + pnl_long)*(1-zeta)

    # check for stop loss
    gross_cash = short_pos.entry_cash*2
    if (pnl < 0) and s:
        if (np.abs(pnl)/gross_cash) > s:
            if not supress_print:
                print('STOP LOSS CONDITION HIT. Closing positions.')
            return [False, 0]

    return [True, pnl]

def close_positions(trade_log: pd.DataFrame, date: str, 
                    short_pos: Position, long_pos: Position, 
                    kxi_price: float, xlp_price: float,
                    zeta: float, reason: str = 'Standard', 
                    supress_print: bool = False):
    '''
    Given dataframe logging trades, short and long positions, date,\n
    prices of kxi and xlp, zeta (trading cost), s, stop loss condition limit,\n
    and reason, reason for closing (defaulted to Standard), \n
    closes positions and logs its data in given dataframe, and returns the dataframe.\n
    supress_print determines if print statements should be supressed.
    '''

    ## get data for log

    # getting position type, price, and amounts
    short_name = short_pos.name
   

    if short_name == 'kxi':
        position_type = 'short'
        kxi_amt = short_pos.amount
        xlp_amt = long_pos.amount
        short_price = kxi_price
        long_price = xlp_price

    else:
        position_type = 'long'
        kxi_amt = long_pos.amount
        xlp_amt = short_pos.amount
        short_price = xlp_price
        long_price = kxi_price
    
    if not supress_print:
        print('Previous position type was: ' + position_type)
    
    # getting cross cash
    gross_cash = short_pos.entry_cash*2

    # getting final pnl
    pnl_ret = get_pnls(short_pos, long_pos, kxi_price, xlp_price, zeta=zeta, s=None, supress_print=supress_print)
    pnl = pnl_ret[1]

    # row to add
    row = [date, 'CLOSE', position_type, gross_cash, pnl, kxi_price, kxi_amt, xlp_price, xlp_amt, reason]
    # add to trade log
    trade_log.loc[len(trade_log)] = row

    # close positions
    short_pos.close(short_price, supress_print)
    long_pos.close(long_price, supress_print)
    if not supress_print:
        print(f'Overall position pnl: {[pnl]}')
    
    return trade_log

def open_positions(trade_log: pd.DataFrame, 
                    short_pos: Position, long_pos: Position,
                    kxi_price: float, xlp_price: float, 
                    date: str, entry_cash: float,
                    position_type: str, supress_print: bool):
    '''
    Given dataframe logging trades, short and long positions, date,\n
    prices of kxi and xlp, entry_cash, the size of transaction, and position type, either short or long,\n
    opens positions and logs its data in given dataframe, and returns the dataframe.\n
    supress_print determines if print statements should be supressed.\n
    Raises ValueError if given spread position is not one of 'short' and 'long.
    '''
    
    if position_type not in ['short', 'long']:
        raise ValueError(f'Invalid spread position: {position_type}')


    # open positions and declare variables based on spread position type
    if position_type == 'long':
        long_pos.open(name='kxi', date=date, entry_cash=entry_cash, price=kxi_price, supress_print=supress_print)
        short_pos.open(name='xlp', date=date, entry_cash=entry_cash, price=xlp_price, supress_print=supress_print)
        kxi_amt = long_pos.amount
        xlp_amt = short_pos.amount
    else:
        long_pos.open(name='xlp', date=date, entry_cash=entry_cash, price=xlp_price, supress_print=supress_print)
        short_pos.open(name='kxi', date=date, entry_cash=entry_cash, price=kxi_price, supress_print=supress_print)
        kxi_amt = short_pos.amount
        xlp_amt = long_pos.amount

    # row to add
    row = [date, 'OPEN', position_type, entry_cash*2, np.nan, kxi_price, kxi_amt, xlp_price, xlp_amt, None]
    # add to trade log
    trade_log.loc[len(trade_log)] = row

    return trade_log

def get_mday_returns(df, row, m):
    '''
    Given time series, today's row, and m, returns
        m-day returns of KXI and XLP.

    returns: [kxi_ret, xlp_ret]
    '''
    # get current date
    date = row.date
    # get current prices of kxi and xlp
    kxi = row.KXI_adj_close
    xlp = row.XLP_adj_close
    # make sure df is sorted by date and index is proper
    df = df.sort_values('date').reset_index(drop=True)
    # move backwards m days
    subdf = df.loc[df[df.date == date].index.values[0]-m]
    # get subdf's prices of kxi and xlp
    old_kxi = subdf.KXI_adj_close
    old_xlp = subdf.XLP_adj_close
    # find returns
    kxi_ret = kxi/old_kxi - 1 
    xlp_ret = xlp/old_xlp - 1
    return [kxi_ret, xlp_ret]



def simulate(trading_df, g, j, s, M, zeta, supress_print = False):
    '''
    Given dataframe of time series of prices of kxi and xlp ETFs, and\n
    given parameters:\n
    g: position entry condition \n
    j: position exit condition \n
    s: stop loss condition \n
    M: duration of return e.g. M-day return on X and Y\n
    zeta: trading cost --> z one of 0, 0.00001

    Runs trading simulation, and logs all trades in a dataframe, and returns dataframe.\n
    supress_print determines if printing should be supressed. 
    '''
    if not supress_print:
        print(f'params: \n{M=}\n{g=}\n{j=}\n{s=}')

    trade_log_cols = ['date', 'position_status', 'position_type', 'gross_cash', 'pnl', 'kxi_price', 'kxi_amount', 'xlp_price', 'xlp_amount', 'Reason']
    trade_log = pd.DataFrame(columns=trade_log_cols)
    stop_loss_date = ''
    long_pos = Position('long')
    short_pos = Position('short')
    # start the clock
    for i, row in trading_df.iterrows():        
        # get current period date
        date = row.date
        # only trade during trade period
        if date < '2022-01-01':
            continue

        # stop loss month skipping logic
        if stop_loss_date:
            if date == stop_loss_date:
                stop_loss_date = ''
            else:
                continue

        # get current period prices
        kxi = row.KXI_adj_close
        xlp = row.XLP_adj_close
        N_t = row.N_t

        # trade cash for A and B each
        trade_cash = row.N_t/100

        if not supress_print:
            print(date)
            print(f'{kxi=}\n{xlp=}\n{N_t=}', end='\n\n')
            print(f'Trade amount: 2*N_t/100 = {2*trade_cash}', end='\n\n')

        # get position pnl and do stop loss check
        if long_pos.is_open() and short_pos.is_open():
            pnl_res = get_pnls(short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, s=s, supress_print=supress_print)
            if not pnl_res[0]:
                if not supress_print:
                    print('Closing from stop loss:')
                stop_loss_date = (datetime.strptime(date, '%Y-%m-%d') + delta(months=1)).strftime('%Y-%m-%d')
                trade_log = close_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, reason='STOP LOSS', supress_print=supress_print)
                if not supress_print:
                    print(20*'-')
                continue

        # check if entering new positions:
        # 1. find m-day returns
        rets = get_mday_returns(trading_df, row, M)
        if not supress_print:
            print(f'{rets=}')
            print(20*'*')
        # 2. find difference of returns
        z = rets[0] - rets[1]
        if not supress_print:
            print(f'{z=}')
        #### rets[0] - rets[1] = z --> kxi - xlp
        # 3. check entry/exit conditions
        if np.abs(z) > g:
            if not supress_print:
                print('entering position.')
            # determine if long or short
            if z > g:
                if not long_pos.is_open() and not short_pos.is_open():
                    if not supress_print:
                        print('shorting spread.')
                    trade_log = open_positions(trade_log = trade_log, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp,
                                   date=date, entry_cash=trade_cash, position_type='short', supress_print=supress_print)
                else:
                    pos_type = 'short' if short_pos.name == 'kxi' else 'long'
                    if pos_type == 'short':
                        if not supress_print:
                            print('Already shorting. Holding position.', end='\n'+20*'-'+'\n')
                        continue
                    else:
                        if not supress_print:
                            print('Flipping position to short.')
                        trade_log = close_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, supress_print=supress_print)
                        # flip
                        trade_log = open_positions(trade_log=trade_log, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp,
                                   date=date, entry_cash=trade_cash, position_type='short', supress_print=supress_print)
            elif z < -g:
                if not long_pos.is_open() and not short_pos.is_open():
                    if not supress_print:
                        print('buying spread.')
                    trade_log = open_positions(trade_log = trade_log, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp,
                                   date=date, entry_cash=trade_cash, position_type='long', supress_print=supress_print)
                else:
                    pos_type = 'long' if long_pos.name == 'kxi' else 'short'
                    if pos_type == 'long':
                        if not supress_print:
                            print('Already long spread. Holding position.', end='\n'+20*'-'+'\n')
                        continue
                    else:
                        if not supress_print:
                            print('Flipping position to long.')
                        trade_log = close_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, supress_print=supress_print)
                        # flip
                        trade_log = open_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp,
                                  entry_cash=trade_cash, position_type='long', supress_print=supress_print)
        elif (np.abs(z) < j) and long_pos.is_open() and short_pos.is_open():
            if not supress_print:
                print('flatenning position.')
            trade_log = close_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, supress_print=supress_print)
        if not supress_print:
            print(20*'-')


    # times up, close positions
    if not supress_print:
        print('Period ended. Closing any open positions.')
    if short_pos.is_open() or long_pos.is_open():
        trade_log = close_positions(trade_log=trade_log, date=date, short_pos=short_pos, long_pos=long_pos, kxi_price=kxi, xlp_price=xlp, zeta=zeta, supress_print=supress_print)
    if not supress_print:
        print(f'Cumulative pnl: {trade_log.pnl.sum()}')
    return trade_log