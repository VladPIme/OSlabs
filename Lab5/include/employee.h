#ifndef EMPLOYEE_H
#define EMPLOYEE_H

// Структура данных о сотруднике
struct Employee {
    int num;          // идентификационный номер сотрудника
    char name[10];    // имя сотрудника
    double hours;     // количество отработанных часов
};

#endif // EMPLOYEE_H