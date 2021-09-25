/**
 *@file DBMigrationList.hpp
 * @author Fabio Rossini Sluzala ()
 * @brief Prepara e roda todas as migrações para a API
 * @version 0.1
 * @date 2021-08-21
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef DBMIGRATIONLIST_HPP
#define DBMIGRATIONLIST_HPP

/**
 *@brief Efetua todo o procedimento e migra as tabelas do banco de dados da API
 *
 */
void prepare_and_run_all_migrations(bool fresh);

#endif
