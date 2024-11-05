#include <iostream>

int maxHP;
int currentHP;

void setMaxHP(int value)
{
    maxHP = value;
}

int getMaxHP()
{
    return maxHP;
}

void setCurrentHP(int value)
{
    currentHP = value;
}

int getCurrentHP()
{
    return currentHP;
}

void takeDamage(int damage)
{
    currentHP -= damage;
    if (currentHP < 0)
    {
        currentHP = 0;
    }
}

void heal(int amount)
{
    currentHP += amount;
    if (currentHP > maxHP)
    {
        currentHP = maxHP;
    }
}

bool isAlive()
{
    return currentHP > 0;
}
// main() showing how it work
/*
int main() {
    // Set the maximum HP
    setMaxHP(100);

    // Set the current HP
    setCurrentHP(80);

    // Take 20 damage
    takeDamage(20);

    // Print the current HP
    std::cout << "Current HP: " << getCurrentHP() << std::endl;

    // Heal the player by 10
    heal(10);

    // Print the current HP again
    std::cout << "Current HP: " << getCurrentHP() << std::endl;

    // Check if the player is alive
    if (isAlive()) {
        std::cout << "The player is alive!" << std::endl;
    } else {
        std::cout << "The player is dead!" << std::endl;
    }

    return 0;
}
*/
