# HarryMakes' Backtesting -- *A Beginner's Backtesting Program*

This project implements a trend-following trading strategy using 20-day and 50-day Simple Moving Averages (SMA) on [Kraken](https://pro.kraken.com/app/trade/) OHLC data.

## MovingAvgStrategy

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
