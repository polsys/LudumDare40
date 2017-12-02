weather = 80
repeat_customers = 0
money = 200

for day in range(1,8):
    # Input
    print("Today is day", day)
    if (day > 5):
        print("It is weekend!")
    print("Weather is", weather)
    print("You have §", money, sep=None)
    price = int(input("-- Price  (integer): "))
    amount = int(input("-- Amount (integer): "))
    print("Making", amount, "glasses of lemonade for §", amount * 2)
    money = money - (amount * 2)
    print("You now have §", money)

    # Customers
    # weather=20, fair_price=0; weather=100, fair_price=7
    fair_price = (-7/4) + (7/80) * weather
    if (day > 5):
        fair_price = fair_price + 1
    print("Customers think the price should be", fair_price)
    customer_factor = fair_price / price
    potential_customers = (100 + repeat_customers) * customer_factor
    customers = int(potential_customers)
    if customers < 0:
        customers = 0
    print("You have", customers, "customers")

    if (amount < customers):
        customers = amount
        print("However, you only made lemonade for", customers)

    # Money!
    print("They happily give you §", customers*price)
    money = money + (customers * price)

    # Repeat customers, depending on price
    # price=1, factor=0.5; price=8, factor=-0.2
    repeat_factor = (-1/10)*price + 0.6
    repeat_customers = repeat_customers + int(customers * repeat_factor)
    print(repeat_customers, "will come back tomorrow!")

    # Weather
    # customers=0, weather=100; customers=200,weather=0
    weather = 100 + (-100/200)*potential_customers

    print()

print("GAME OVER. YOUR SCORE: §", int(money))
