// ������:
// �������� ����� ����������� �������� � �������� ������
// ������������� ��������: 
// �������� ������� ������ �� �����, 
// ��������� ����,
// ����� �� �����.
// ������ �� ����� �� ������ ���� �������������
// ������������� ������ ������ � ������������� ����������

#include <iostream>
#include <atomic>
#include <string>
#include <cstdint>
#include <limits>

void showUserMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

class Account
{
public:
    Account(std::uint64_t id): mId{id}, mAccountAmount{0}
    {
    }

    std::uint64_t currentAmount() const
    {
        // ���������� ��������� �������� ��� ��������� ��������
        return mAccountAmount.load(); 
    }

    bool addMoney(std::uint64_t amountToAdd)
    {
        std::uint64_t currentAmount = mAccountAmount.load();
        std::uint64_t newAmount;
    
        while (true)
        {
            // �������� �� ������������
            if (std::numeric_limits<std::uint64_t>::max() - currentAmount < amountToAdd)
            {
                showUserMessage("balance overflow");
                return false;
            }

            // �������� �������� ��������, ��������� ��������� ��������
            newAmount = currentAmount + amountToAdd;
            if (mAccountAmount.compare_exchange_weak(currentAmount, newAmount))
            {
                return true;
            }

            // ��������� currentAmount ��� ��������� �������
            currentAmount = mAccountAmount.load();
        }
    }

    bool getMoney(std::uint64_t amountToGet)
    {
        std::uint64_t currentAmount = mAccountAmount.load();
        std::uint64_t newAmount;

        while (true)
        {
            if (currentAmount < amountToGet)
            {
                showUserMessage("not enough funds on the account");
                return false;
            }

            newAmount = currentAmount - amountToGet;
            if (mAccountAmount.compare_exchange_weak(currentAmount, newAmount))
            {
                return true;
            }

            currentAmount = mAccountAmount.load();
        }
    }

private:
    std::uint64_t mId;
    std::atomic<std::uint64_t> mAccountAmount;
};


int main()
{
    Account acc(223);
    acc.addMoney(100);
    std::cout << "Current balance: " << acc.currentAmount() << std::endl;

    if (acc.getMoney(50))
    {
        std::cout << "Withdraw successful" << std::endl;
    }
    else
    {
        std::cout << "Withdraw failed" << std::endl;
    }

    std::cout << "Current balance: " << acc.currentAmount() << std::endl;

    if (acc.getMoney(60))
    {
        std::cout << "Withdraw successful" << std::endl;
    }
    else
    {
        std::cout << "Withdraw failed" << std::endl;
    }

    std::cout << "Current balance: " << acc.currentAmount() << std::endl;

    return 0;
}