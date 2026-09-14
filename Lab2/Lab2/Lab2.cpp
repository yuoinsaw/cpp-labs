//  Лабораторная работа №2
//
//  Условие: В каждом решении должна быть иерархия классов (как минимум одна)
//  и полиморфизм.
//
//  Вариант 6: Защита базы.
//
//  База представляет собой крепость, по периметру которой расставлены дроиды.
//  Типы объектов: Генерал Гривус, дроид-штурмовик, дроид-разведчик, дроид-рабочий,
//  базовый дроид и повстанцы. Каждый представитель является отдельным экземпляром
//  класса.
//
//  База имеет фиксированный объём энергии. Генерал Гривус потребляет много энергии
//  и создаёт базовых дроидов. Рано или поздно базовый дроид может быть переопределён
//  в штурмовика, разведчика или рабочего — это случайным образом решает Генерал
//  Гривус. Если на базе закончится энергия, считается, что база будет захвачена
//  повстанцами.
//
//  Штурмовики отражают атаки повстанцев и потребляют много энергии; разведчики
//  помогают увеличить число добываемой энергии и потребляют меньше штурмовика;
//  рабочие добывают энергию и потребляют меньше разведчиков; базовые дроиды
//  только потребляют энергию.
//
//  Параметры объектов:
//    Генерал Гривус  — количество потребляемой энергии, количество созданных
//                      разведчиков;
//    штурмовики      — количество потребляемой энергии, количество уничтожаемых
//                      повстанцев;
//    разведчики      — количество потребляемой энергии, процент, на который
//                      увеличивается добыча энергии;
//    рабочие         — количество потребляемой энергии, количество добываемой
//                      энергии;
//    базовые дроиды  — количество потребляемой энергии.
//
//  Необходимо показать работу базы сепаратистов для различных значений числа
//  дроидов, повстанцев и энергии. Повстанцы неотличимы по классам и имеют
//  случайный радиус действия оружия и степень урона.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

using namespace std;

const double GRIEVOUS_USE      = 12.0;
const double DROID_BUILD_COST  = 12.0;
const double ENERGY_RESERVE    = 120.0;

const double BASIC_USE         = 3.0;
const double WORKER_USE        = 4.0;
const double WORKER_MINING     = 20.0;
const double SCOUT_USE         = 6.0;
const double SCOUT_BONUS       = 8.0;
const double STORM_USE         = 8.0;
const int    STORM_KILLS       = 1;

const int    PROMOTION_CHANCE  = 55;
const int    MAX_TURNS         = 40;

class Base;

int randRange(int a, int b) { return a + rand() % (b - a + 1); }

class Unit {
protected:
    string name;
    int    id;
    double energyPerTurn;
    static int counter;
public:
    Unit(const string& n, double use) : name(n), energyPerTurn(use) {
        id = ++counter;
    }
    virtual ~Unit() {}

    virtual void   act(Base& base) = 0;
    virtual string role() const = 0;

    virtual string info() const {
        return "#" + to_string(id) + " " + name;
    }

    double        getEnergyUse() const { return energyPerTurn; }
    const string& getName()      const { return name; }
    int           getId()        const { return id; }

    static int totalCreated() { return counter; }
};
int Unit::counter = 0;

class Droid : public Unit {
public:
    Droid(const string& n, double use) : Unit(n, use) {}
    virtual char symbol() const = 0;
};

class BasicDroid : public Droid {
public:
    BasicDroid() : Droid("Базовый дроид", BASIC_USE) {}
    void   act(Base& base) override;
    string role()   const override { return "базовый"; }
    char   symbol() const override { return 'b'; }
};

class WorkerDroid : public Droid {
private:
    double mining;
public:
    WorkerDroid() : Droid("Дроид-рабочий", WORKER_USE), mining(WORKER_MINING) {}
    void   act(Base& base) override;
    string role()   const override { return "рабочий"; }
    char   symbol() const override { return 'W'; }
    string info()   const override {
        return Unit::info() + " (добыча " + to_string((int)mining) + ")";
    }
};

class ScoutDroid : public Droid {
private:
    double bonusPercent;
public:
    ScoutDroid() : Droid("Дроид-разведчик", SCOUT_USE), bonusPercent(SCOUT_BONUS) {}
    void   act(Base& base) override;
    string role()   const override { return "разведчик"; }
    char   symbol() const override { return 'S'; }
    string info()   const override {
        return Unit::info() + " (+" + to_string((int)bonusPercent) + "% к добыче)";
    }
};

class StormDroid : public Droid {
private:
    int kills;
public:
    StormDroid() : Droid("Дроид-штурмовик", STORM_USE), kills(STORM_KILLS) {}
    void   act(Base& base) override;
    string role()   const override { return "штурмовик"; }
    char   symbol() const override { return 'A'; }
    string info()   const override {
        return Unit::info() + " (уничтожает " + to_string(kills) + " за ход)";
    }
};

class GeneralGrievous : public Unit {
private:
    int droidsCreated;
    int scoutsCreated;
public:
    GeneralGrievous()
        : Unit("Генерал Гривус", GRIEVOUS_USE), droidsCreated(0), scoutsCreated(0) {}

    void   act(Base& base) override;
    void   promoteDroids(Base& base);
    string role()   const override { return "генерал"; }
    string info()   const override {
        return Unit::info() + " (создано дроидов: " + to_string(droidsCreated)
             + ", из них разведчиков: " + to_string(scoutsCreated) + ")";
    }
};

class Rebel : public Unit {
private:
    int weaponRadius;
    int damage;
    int distance;
public:
    Rebel() : Unit("Повстанец", 0.0) {
        weaponRadius = randRange(3, 12);
        damage       = randRange(5, 20);
        distance     = randRange(10, 16);
    }
    void   act(Base& base) override;
    string role() const override { return "повстанец"; }
    string info() const override {
        return Unit::info() + " (радиус " + to_string(weaponRadius)
             + ", урон " + to_string(damage) + ", дистанция " + to_string(distance) + ")";
    }
    int  getDistance() const { return distance; }
    bool inRange()     const { return distance <= weaponRadius; }
};

class Base {
private:
    double energy;
    double miningPool;
    double scoutBonus;
    int    firepower;
    double damageTaken;

    vector<Unit*> garrison;
    vector<Unit*> rebels;
    vector<Unit*> spawnQueue;

    GeneralGrievous* general;
    vector<string>   events;
    int turn;

public:
    Base(double startEnergy, int startDroids, int rebelCount);
    ~Base();

    void addMining(double v)     { miningPool += v; }
    void addScoutBonus(double p) { scoutBonus += p; }
    void addFirepower(int k)     { firepower  += k; }
    void spendEnergy(double v)   { energy     -= v; }
    void takeDamage(double d)    { energy -= d; damageTaken += d; }
    void queueDroid(Unit* d)     { spawnQueue.push_back(d); }
    void log(const string& s)    { events.push_back(s); }

    double         getEnergy()   const { return energy; }
    vector<Unit*>& getGarrison()       { return garrison; }

    bool run(int maxTurns);

private:
    void tick(double& minedOut);
    void consumeEnergy();
    void battle();
    void printHeader() const;
    void printRow(double mined) const;
    void printGarrison() const;
    int  countOf(const string& r) const;
};

void BasicDroid::act(Base&) {}
void WorkerDroid::act(Base& base) { base.addMining(mining); }
void ScoutDroid::act(Base& base) { base.addScoutBonus(bonusPercent); }
void StormDroid::act(Base& base) { base.addFirepower(kills); }

void GeneralGrievous::act(Base& base) {
    if (base.getEnergy() < ENERGY_RESERVE) return;

    int built = 0;
    int planned = randRange(1, 2);
    for (int i = 0; i < planned; i++) {
        if (base.getEnergy() - DROID_BUILD_COST < ENERGY_RESERVE) break;
        base.spendEnergy(DROID_BUILD_COST);
        base.queueDroid(new BasicDroid());
        droidsCreated++;
        built++;
    }
    if (built > 0) base.log("Гривус построил базовых дроидов: " + to_string(built));
}

void GeneralGrievous::promoteDroids(Base& base) {
    vector<Unit*>& g = base.getGarrison();
    for (size_t i = 0; i < g.size(); i++) {
        BasicDroid* basic = dynamic_cast<BasicDroid*>(g[i]);
        if (basic == nullptr) continue;
        if (randRange(1, 100) > PROMOTION_CHANCE) continue;

        Unit* replacement = nullptr;
        switch (randRange(1, 3)) {
            case 1:  replacement = new StormDroid();  break;
            case 2:  replacement = new ScoutDroid();  scoutsCreated++; break;
            default: replacement = new WorkerDroid(); break;
        }
        base.log("Дроид #" + to_string(basic->getId()) + " переопределён -> "
                 + replacement->role());
        delete g[i];
        g[i] = replacement;
    }
}

void Rebel::act(Base& base) {
    if (!inRange()) {
        distance--;
    } else {
        base.takeDamage(damage);
    }
}

Base::Base(double startEnergy, int startDroids, int rebelCount)
    : energy(startEnergy), miningPool(0), scoutBonus(0), firepower(0),
      damageTaken(0), general(nullptr), turn(0)
{
    general = new GeneralGrievous();
    garrison.push_back(general);
    for (int i = 0; i < startDroids; i++) garrison.push_back(new BasicDroid());
    for (int i = 0; i < rebelCount;  i++) rebels.push_back(new Rebel());
}

Base::~Base() {
    for (size_t i = 0; i < garrison.size();   i++) delete garrison[i];
    for (size_t i = 0; i < rebels.size();     i++) delete rebels[i];
    for (size_t i = 0; i < spawnQueue.size(); i++) delete spawnQueue[i];
}

int Base::countOf(const string& r) const {
    int n = 0;
    for (size_t i = 0; i < garrison.size(); i++)
        if (garrison[i]->role() == r) n++;
    return n;
}

void Base::consumeEnergy() {
    double total = 0;
    for (size_t i = 0; i < garrison.size(); i++)
        total += garrison[i]->getEnergyUse();
    energy -= total;
}

void Base::battle() {
    if (firepower > 0 && !rebels.empty()) {
        sort(rebels.begin(), rebels.end(), [](Unit* a, Unit* b) {
            return dynamic_cast<Rebel*>(a)->getDistance()
                 < dynamic_cast<Rebel*>(b)->getDistance();
        });
        size_t killed = min((size_t)firepower, rebels.size());
        for (size_t i = 0; i < killed; i++) delete rebels[i];
        rebels.erase(rebels.begin(), rebels.begin() + killed);
        log("Штурмовики уничтожили повстанцев: " + to_string(killed));
    }
    for (size_t i = 0; i < rebels.size(); i++) rebels[i]->act(*this);
}

void Base::tick(double& minedOut) {
    miningPool = 0; scoutBonus = 0; firepower = 0; damageTaken = 0;
    events.clear();

    for (size_t i = 0; i < garrison.size(); i++) garrison[i]->act(*this);

    minedOut = miningPool * (1.0 + scoutBonus / 100.0);
    energy += minedOut;

    general->promoteDroids(*this);

    for (size_t i = 0; i < spawnQueue.size(); i++) garrison.push_back(spawnQueue[i]);
    spawnQueue.clear();

    consumeEnergy();
    battle();
}

void Base::printHeader() const {
    cout << "\n  Ход | Энергия | Ген | Баз | Раб | Разв | Штурм | Повст | Добыто | Урон\n";
    cout << "  ----+---------+-----+-----+-----+------+-------+-------+--------+-----\n";
}

void Base::printRow(double mined) const {
    cout << setw(5) << turn << " |"
         << setw(8) << (int)energy << " |"
         << setw(4) << countOf("генерал")   << " |"
         << setw(4) << countOf("базовый")   << " |"
         << setw(4) << countOf("рабочий")   << " |"
         << setw(5) << countOf("разведчик") << " |"
         << setw(6) << countOf("штурмовик") << " |"
         << setw(6) << rebels.size()        << " |"
         << setw(7) << (int)mined           << " |"
         << setw(5) << (int)damageTaken     << "\n";
    for (size_t i = 0; i < events.size(); i++)
        cout << "        - " << events[i] << "\n";
}

void Base::printGarrison() const {
    cout << "  Гарнизон (G-генерал, b-базовый, W-рабочий, S-разведчик, A-штурмовик):\n  ";
    for (size_t i = 0; i < garrison.size(); i++) {
        Droid* d = dynamic_cast<Droid*>(garrison[i]);
        cout << (d ? d->symbol() : 'G');
    }
    cout << "\n";
}

bool Base::run(int maxTurns) {
    cout << "\nНачальное состояние: энергия " << (int)energy
         << ", гарнизон " << garrison.size()
         << ", повстанцев " << rebels.size() << "\n";
    printGarrison();
    printHeader();

    bool survived = true;
    for (turn = 1; turn <= maxTurns; turn++) {
        double mined = 0;
        tick(mined);
        printRow(mined);

        if (energy <= 0) {
            cout << "\n  >>> ЭНЕРГИЯ ИСЧЕРПАНА. База захвачена повстанцами на ходу "
                 << turn << ". <<<\n";
            survived = false;
            break;
        }
        if (rebels.empty()) {
            cout << "\n  >>> Все повстанцы уничтожены на ходу " << turn
                 << ". База выстояла. <<<\n";
            break;
        }
    }
    if (survived && !rebels.empty())
        cout << "\n  >>> Лимит ходов исчерпан. База держится, осада продолжается. <<<\n";

    cout << "\nИтог: энергия " << (int)energy
         << ", гарнизон " << garrison.size()
         << ", повстанцев осталось " << rebels.size() << "\n";
    printGarrison();
    cout << "  " << general->info() << "\n";
    cout << "  Всего объектов создано за симуляцию: " << Unit::totalCreated() << "\n";
    return survived;
}

int readInt(const char* prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Ошибка: нужно ввести целое число.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (value < minValue || value > maxValue) {
            cout << "  Ошибка: допустим диапазон от " << minValue
                 << " до " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

void runScenario(const char* title, int energy, int droids, int rebels) {
    cout << "\n============================================================\n";
    cout << " СЦЕНАРИЙ: " << title << "\n";
    cout << "============================================================\n";
    Base base(energy, droids, rebels);
    base.run(MAX_TURNS);
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    srand(static_cast<unsigned>(time(nullptr)));

    while (true) {
        cout << "\n--- Меню ---\n"
             << " 1 - Сценарий \"Патруль\"          (энергия 1200, дроидов 6, повстанцев 8)\n"
             << " 2 - Сценарий \"Осада\"            (энергия 1000, дроидов 5, повстанцев 25)\n"
             << " 3 - Сценарий \"Последний рубеж\"  (энергия 400,  дроидов 3, повстанцев 40)\n"
             << " 4 - Задать свои параметры\n"
             << " 0 - Выход\n";
        int choice = readInt("Ваш выбор: ", 0, 4);

        if (choice == 0) break;
        else if (choice == 1) runScenario("Патруль",         1200, 6,  8);
        else if (choice == 2) runScenario("Осада",           1000, 5,  25);
        else if (choice == 3) runScenario("Последний рубеж",  400, 3,  40);
        else {
            int e = readInt("Начальная энергия базы (100..10000): ", 100, 10000);
            int d = readInt("Начальное число дроидов (0..50):     ", 0, 50);
            int r = readInt("Число повстанцев        (1..200):    ", 1, 200);
            runScenario("Свои параметры", e, d, r);
        }
    }

    cout << "\nРабота завершена.\n";
    return 0;
}
