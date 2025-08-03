#include "database_pool.h"
#ifdef MSTL_SUPPORT_MYSQL__
MSTL_BEGIN_NAMESPACE__
_MSTL string database_settings::username = "root";
_MSTL string database_settings::password = "";
_MSTL string database_settings::dbname = "";
_MSTL string database_settings::ip = "127.0.0.1";
_MSTL uint16_t database_settings::port = 3306;
_MSTL string database_settings::character_set = "utf8mb4";

MSTL_END_NAMESPACE__
#endif // MSTL_SUPPORT_MYSQL__
