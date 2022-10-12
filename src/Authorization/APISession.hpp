#pragma once

#include "ApiAccessID.hpp"

struct APISession {
    PersonalAccess user;

    void logout() {
        if (!user.abilities.isNull()) {
            user.abilities->clear();
        }
        
        user.id = 0;
        user.tokenable_id = 0;
        user.token.clear();
    }
};
