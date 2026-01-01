# HarryMakes' Backtesting -- *A Beginner's Backtesting Program*

This project aims to perform backtesting on [Kraken](https://pro.kraken.com/app/trade/). It implements multiple trend-following trading strategies:

- `moving_avg`: Moving Averages
- `weinstein`: Weinstein's Four Stage Analysis

## `moving_avg`: Moving Averages

This is a trend-following trading strategy using 20-day and 50-day Simple Moving Averages (SMA) on **Closing Price** data.

### Parameters

- `short_period`: The shorter period for calculating Simple Moving Average. Typically 20 days.
- `long_period`: The longer period for calculating Simple Moving Average. Typically 50 days.

### Strategy

- Long Strategy
  - **Golden Cross**: When the 20-day SMA crosses above the 50-day SMA at time Tb1, signaling a potential uptrend.
  - **Death Cross**: When the 20-day SMA crosses below the 50-day SMA at Tb2, ending the trend.
  - **Condition**: The trend (Tb2 - Tb1) must last over 3 months to confirm strength.
  - **Next Golden Cross**: Must occur at Tb3, before Tb2 + (Tb2 - Tb1)/2, indicating an early trend reversal.
  - **Price Confirmation**: Post-Tb3, the price must reach or exceed the highest price between Tb1 and Tb2.
  - **Action**: Enter a Long position (buy) upon price confirmation.

- Short Strategy
  - **Death Cross**: When the 20-day SMA crosses below the 50-day SMA at Ts1, signaling a potential downtrend.
  - **Golden Cross**: When the 20-day SMA crosses above the 50-day SMA at Ts2, ending the trend.
  - **Condition**: The trend (Ts2 - Ts1) must last over 3 months.
  - **Next Death Cross**: Must occur at Ts3, before Ts2 + (Ts2 - Ts1)/2.
  - **Price Confirmation**: Post-Ts3, the price must fall to or below the lowest price between Ts1 and Ts2.
  - **Action**: Enter a Short position (sell) upon price confirmation.

### Why It Works
The strategy filters noise by requiring a 3-month trend, confirms reversals with a mid-term cross, and validates trades with price breakouts, improving signal quality and capturing momentum in trending markets.

## `weinstein`: Weinstein's Four Stage Analysis

[TBD]

---

# How-to

1. `cmake --preset debug`
2. `cmake --build out/build/debug -j $(nproc) --target all --`

# Licence

This project is to be licensed under **GNU General Public License v3.0**. See `LICENSE`.

This project incorporates part of following program(s):

- https://github.com/perlawk/zzindic \
  ```text
  /*
    TTR: Technical Trading Rules

    Copyright (C) 2007-2013  Joshua M. Ulrich

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
  ```

- (Reference only) https://github.com/BatuhanUsluel/Algorithmic-Support-and-Resistance \
  ```text
  MIT License

  Copyright (c) 2019 BatuhanU

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  ```
