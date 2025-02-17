#include "count.h"

#include "building/building.h"
#include "building/monument.h"
#include "city/buildings.h"
#include "city/health.h"
#include "figure/figure.h"

#include <stdlib.h>
#include <string.h>

#define ORIGINAL_BUFFER_SIZE_CULTURE1 132
#define CURRENT_BUFFER_SIZE_CULTURE1 160
#define ORIGINAL_BUFFER_SIZE_CULTURE2 32
#define CURRENT_BUFFER_SIZE_CULTURE2 32
#define ORIGINAL_BUFFER_SIZE_CULTURE3 40
#define CURRENT_BUFFER_SIZE_CULTURE3 40
#define ORIGINAL_BUFFER_SIZE_MILITARY 16
#define CURRENT_BUFFER_SIZE_MILITARY 16
#define ORIGINAL_BUFFER_SIZE_SUPPORT 24
#define CURRENT_BUFFER_SIZE_SUPPORT 24
#define ORIGINAL_BUFFER_SIZE_INDUSTRY 128
#define CURRENT_BUFFER_SIZE_INDUSTRY 128




typedef enum {
    BUFFER_INDUSTRY,
    BUFFER_CULTURE_1,
    BUFFER_CULTURE_2,
    BUFFER_CULTURE_3,
    BUFFER_MILITARY,
    BUFFER_SUPPORT,
} count_buffers;


struct record {
    int active;
    int total;
    int upgraded;
};

static struct {
    struct record buildings[BUILDING_TYPE_MAX];
    struct record industry[RESOURCE_MAX];
} data;

static void clear_counters(void)
{
    memset(&data, 0, sizeof(data));
}

static void increase_count(building_type type, int active, int upgraded)
{
    ++data.buildings[type].total;
    if (active) {
        ++data.buildings[type].active;
        if (upgraded) {
            ++data.buildings[type].upgraded;
        }
    }
}

static void increase_industry_count(resource_type resource, int active)
{
    ++data.industry[resource].total;
    if (active) {
        ++data.industry[resource].active;
    }
}

static void limit_hippodrome(void)
{
    if (data.buildings[BUILDING_HIPPODROME].total > 1) {
        data.buildings[BUILDING_HIPPODROME].total = 1;
    }
    if (data.buildings[BUILDING_COLOSSEUM].active > 1) {
        data.buildings[BUILDING_COLOSSEUM].active = 1;
    }
}

void building_count_update(void)
{
    clear_counters();
    city_buildings_reset_dock_wharf_counters();
    city_health_reset_hospital_workers();

    for (int i = 1; i < building_count(); i++) {
        building *b = building_get(i);
        if (b->state != BUILDING_STATE_IN_USE || b->house_size) {
            continue;
        }
        int is_entertainment_venue = 0;
        int type = b->type;
        switch (type) {
            // SPECIAL TREATMENT
            // entertainment venues
            case BUILDING_THEATER:
            case BUILDING_AMPHITHEATER:
            case BUILDING_COLOSSEUM:
            case BUILDING_HIPPODROME:
            case BUILDING_ARENA:
                is_entertainment_venue = 1;
                increase_count(type, b->num_workers > 0, b->upgrade_level);
                break;

            case BUILDING_BARRACKS:
                city_buildings_set_barracks(i);
                increase_count(type, b->num_workers > 0, b->upgrade_level);
                break;

            case BUILDING_HOSPITAL:
                increase_count(type, b->num_workers > 0, b->upgrade_level);
                city_health_add_hospital_workers(b->num_workers);
                break;

            // water
            case BUILDING_RESERVOIR:
            case BUILDING_FOUNTAIN:
                increase_count(type, b->has_water_access, b->upgrade_level);
                break;

            // DEFAULT TREATMENT
            // education
            case BUILDING_SCHOOL:
            case BUILDING_LIBRARY:
            case BUILDING_ACADEMY:
            // health
            case BUILDING_BARBER:
            case BUILDING_BATHHOUSE:
            case BUILDING_DOCTOR:
            // government
            case BUILDING_FORUM:
            case BUILDING_FORUM_UPGRADED:
            case BUILDING_SENATE:
            case BUILDING_SENATE_UPGRADED:
            // entertainment schools
            case BUILDING_ACTOR_COLONY:
            case BUILDING_GLADIATOR_SCHOOL:
            case BUILDING_LION_HOUSE:
            case BUILDING_CHARIOT_MAKER:
            // distribution
            case BUILDING_MARKET:
            case BUILDING_TAVERN:
            case BUILDING_CARAVANSERAI:
            // military
            case BUILDING_MILITARY_ACADEMY:
            case BUILDING_MESS_HALL:
            // religion
            case BUILDING_SMALL_TEMPLE_CERES:
            case BUILDING_SMALL_TEMPLE_NEPTUNE:
            case BUILDING_SMALL_TEMPLE_MERCURY:
            case BUILDING_SMALL_TEMPLE_MARS:
            case BUILDING_SMALL_TEMPLE_VENUS:
            case BUILDING_LARGE_TEMPLE_CERES:
            case BUILDING_LARGE_TEMPLE_NEPTUNE:
            case BUILDING_LARGE_TEMPLE_MERCURY:
            case BUILDING_LARGE_TEMPLE_MARS:
            case BUILDING_LARGE_TEMPLE_VENUS:
            case BUILDING_GRAND_TEMPLE_CERES:
            case BUILDING_GRAND_TEMPLE_NEPTUNE:
            case BUILDING_GRAND_TEMPLE_MERCURY:
            case BUILDING_GRAND_TEMPLE_MARS:
            case BUILDING_GRAND_TEMPLE_VENUS:
            case BUILDING_PANTHEON:
            case BUILDING_ORACLE:
            case BUILDING_LARARIUM:
            case BUILDING_NYMPHAEUM:
            case BUILDING_SMALL_MAUSOLEUM:
            case BUILDING_LARGE_MAUSOLEUM:
                increase_count(type, b->num_workers > 0, b->upgrade_level);
                break;

            // industry
            case BUILDING_WHEAT_FARM:
                increase_industry_count(RESOURCE_WHEAT, b->num_workers > 0);
                break;
            case BUILDING_VEGETABLE_FARM:
                increase_industry_count(RESOURCE_VEGETABLES, b->num_workers > 0);
                break;
            case BUILDING_FRUIT_FARM:
                increase_industry_count(RESOURCE_FRUIT, b->num_workers > 0);
                break;
            case BUILDING_OLIVE_FARM:
                increase_industry_count(RESOURCE_OLIVES, b->num_workers > 0);
                break;
            case BUILDING_VINES_FARM:
                increase_industry_count(RESOURCE_VINES, b->num_workers > 0);
                break;
            case BUILDING_PIG_FARM:
                increase_industry_count(RESOURCE_MEAT, b->num_workers > 0);
                break;
            case BUILDING_MARBLE_QUARRY:
                increase_industry_count(RESOURCE_MARBLE, b->num_workers > 0);
                break;
            case BUILDING_IRON_MINE:
                increase_industry_count(RESOURCE_IRON, b->num_workers > 0);
                break;
            case BUILDING_TIMBER_YARD:
                increase_industry_count(RESOURCE_TIMBER, b->num_workers > 0);
                break;
            case BUILDING_CLAY_PIT:
                increase_industry_count(RESOURCE_CLAY, b->num_workers > 0);
                break;
            case BUILDING_WINE_WORKSHOP:
                increase_industry_count(RESOURCE_WINE, b->num_workers > 0);
                break;
            case BUILDING_OIL_WORKSHOP:
                increase_industry_count(RESOURCE_OIL, b->num_workers > 0);
                break;
            case BUILDING_WEAPONS_WORKSHOP:
                increase_industry_count(RESOURCE_WEAPONS, b->num_workers > 0);
                break;
            case BUILDING_FURNITURE_WORKSHOP:
                increase_industry_count(RESOURCE_FURNITURE, b->num_workers > 0);
                break;
            case BUILDING_POTTERY_WORKSHOP:
                increase_industry_count(RESOURCE_POTTERY, b->num_workers > 0);
                break;

            // water-side
            case BUILDING_WHARF:
                if (b->num_workers > 0) {
                    city_buildings_add_working_wharf(!b->data.industry.fishing_boat_id);
                }
                increase_industry_count(RESOURCE_MEAT, b->num_workers > 0 && b->data.industry.fishing_boat_id);
                break;
            case BUILDING_DOCK:
                if (b->num_workers > 0 && b->has_water_access) {
                    city_buildings_add_working_dock(i);
                }
                break;
            default:
                continue;
        }
        if (b->immigrant_figure_id) {
            figure *f = figure_get(b->immigrant_figure_id);
            if (f->state != FIGURE_STATE_ALIVE || f->destination_building_id != i) {
                b->immigrant_figure_id = 0;
            }
        }
        if (is_entertainment_venue && (!building_monument_is_monument(b) || b->data.monument.phase == MONUMENT_FINISHED)) {
            // update number of shows
            int shows = 0;
            if (b->data.entertainment.days1 > 0) {
                --b->data.entertainment.days1;
                ++shows;
            }
            if (b->data.entertainment.days2 > 0) {
                --b->data.entertainment.days2;
                ++shows;
            }
            b->data.entertainment.num_shows = shows;
        }
    }
    limit_hippodrome();
}

int building_count_grand_temples(void)
{
    return (data.buildings[BUILDING_GRAND_TEMPLE_CERES].total +
        data.buildings[BUILDING_GRAND_TEMPLE_NEPTUNE].total +
        data.buildings[BUILDING_GRAND_TEMPLE_MERCURY].total +
        data.buildings[BUILDING_GRAND_TEMPLE_MARS].total +
        data.buildings[BUILDING_GRAND_TEMPLE_VENUS].total);
}

int building_count_grand_temples_active(void)
{
    return (data.buildings[BUILDING_GRAND_TEMPLE_CERES].active +
        data.buildings[BUILDING_GRAND_TEMPLE_NEPTUNE].active +
        data.buildings[BUILDING_GRAND_TEMPLE_MERCURY].active +
        data.buildings[BUILDING_GRAND_TEMPLE_MARS].active +
        data.buildings[BUILDING_GRAND_TEMPLE_VENUS].active +
        data.buildings[BUILDING_PANTHEON].active);
}

int building_count_colosseum(void)
{
    return data.buildings[BUILDING_COLOSSEUM].total;
}

int building_count_active(building_type type)
{
    return data.buildings[type].active;
}

int building_count_total(building_type type)
{
    return data.buildings[type].total;
}

int building_count_upgraded(building_type type)
{
    return data.buildings[type].upgraded;
}

int building_count_industry_active(resource_type resource)
{
    return data.industry[resource].active;
}

int building_count_industry_total(resource_type resource)
{
    return data.industry[resource].total;
}

void building_count_save_state(buffer *industry, buffer *culture1, buffer *culture2,
    buffer *culture3, buffer *military, buffer *support)
{
    int buffer_sizes[] = { CURRENT_BUFFER_SIZE_INDUSTRY,CURRENT_BUFFER_SIZE_CULTURE1,CURRENT_BUFFER_SIZE_CULTURE2,CURRENT_BUFFER_SIZE_CULTURE3, CURRENT_BUFFER_SIZE_MILITARY, CURRENT_BUFFER_SIZE_SUPPORT };
    buffer *buffer_pointers[] = { industry,culture1,culture2,culture3,military,support };
    int buffer_count = 6;


    for (int i = 0; i < buffer_count; i++) {
        buffer *buf = buffer_pointers[i];
        int buf_size = buffer_sizes[i] + 4; // Extra 4 bytes to store buffer size
        uint8_t *buf_data = malloc(buf_size);
        buffer_init(buf, buf_data, buf_size);
        buffer_write_i32(buf, buf_size);
    }
    // industry
    for (int i = 0; i < RESOURCE_MAX; i++) {
        buffer_write_i32(industry, data.industry[i].total);
    }
    for (int i = 0; i < RESOURCE_MAX; i++) {
        buffer_write_i32(industry, data.industry[i].active);
    }

    // culture 1
    buffer_write_i32(culture1, data.buildings[BUILDING_THEATER].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_THEATER].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_AMPHITHEATER].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_AMPHITHEATER].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_COLOSSEUM].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_COLOSSEUM].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_HIPPODROME].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_HIPPODROME].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_SCHOOL].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_SCHOOL].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_LIBRARY].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LIBRARY].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_ACADEMY].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_ACADEMY].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_BARBER].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_BARBER].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_BATHHOUSE].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_BATHHOUSE].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_DOCTOR].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_DOCTOR].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_HOSPITAL].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_HOSPITAL].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_SMALL_TEMPLE_CERES].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_SMALL_TEMPLE_MARS].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_SMALL_TEMPLE_VENUS].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LARGE_TEMPLE_CERES].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LARGE_TEMPLE_MARS].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_LARGE_TEMPLE_VENUS].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_ORACLE].total);

    // Save arena counts upgraded culture ratings buildings

    buffer_write_i32(culture1, data.buildings[BUILDING_ARENA].total);
    buffer_write_i32(culture1, data.buildings[BUILDING_ARENA].active);
    buffer_write_i32(culture1, data.buildings[BUILDING_ARENA].upgraded);
    buffer_write_i32(culture1, data.buildings[BUILDING_THEATER].upgraded);
    buffer_write_i32(culture1, data.buildings[BUILDING_SCHOOL].upgraded);
    buffer_write_i32(culture1, data.buildings[BUILDING_LIBRARY].upgraded);
    buffer_write_i32(culture1, data.buildings[BUILDING_ACADEMY].upgraded);


    // culture 2
    buffer_write_i32(culture2, data.buildings[BUILDING_ACTOR_COLONY].total);
    buffer_write_i32(culture2, data.buildings[BUILDING_ACTOR_COLONY].active);
    buffer_write_i32(culture2, data.buildings[BUILDING_GLADIATOR_SCHOOL].total);
    buffer_write_i32(culture2, data.buildings[BUILDING_GLADIATOR_SCHOOL].active);
    buffer_write_i32(culture2, data.buildings[BUILDING_LION_HOUSE].total);
    buffer_write_i32(culture2, data.buildings[BUILDING_LION_HOUSE].active);
    buffer_write_i32(culture2, data.buildings[BUILDING_CHARIOT_MAKER].total);
    buffer_write_i32(culture2, data.buildings[BUILDING_CHARIOT_MAKER].active);

    // culture 3
    buffer_write_i32(culture3, data.buildings[BUILDING_SMALL_TEMPLE_CERES].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_SMALL_TEMPLE_MARS].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_SMALL_TEMPLE_VENUS].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_LARGE_TEMPLE_CERES].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_LARGE_TEMPLE_MARS].active);
    buffer_write_i32(culture3, data.buildings[BUILDING_LARGE_TEMPLE_VENUS].active);

    // military
    buffer_write_i32(military, data.buildings[BUILDING_MILITARY_ACADEMY].total);
    buffer_write_i32(military, data.buildings[BUILDING_MILITARY_ACADEMY].active);
    buffer_write_i32(military, data.buildings[BUILDING_BARRACKS].total);
    buffer_write_i32(military, data.buildings[BUILDING_BARRACKS].active);

    // support
    buffer_write_i32(support, data.buildings[BUILDING_MARKET].total);
    buffer_write_i32(support, data.buildings[BUILDING_MARKET].active);
    buffer_write_i32(support, data.buildings[BUILDING_RESERVOIR].total);
    buffer_write_i32(support, data.buildings[BUILDING_RESERVOIR].active);
    buffer_write_i32(support, data.buildings[BUILDING_FOUNTAIN].total);
    buffer_write_i32(support, data.buildings[BUILDING_FOUNTAIN].active);
}

void building_count_load_state(buffer *industry, buffer *culture1, buffer *culture2,
    buffer *culture3, buffer *military, buffer *support, int includes_buffer_size)
{
    int buf_sizes[] = { ORIGINAL_BUFFER_SIZE_INDUSTRY,ORIGINAL_BUFFER_SIZE_CULTURE1,ORIGINAL_BUFFER_SIZE_CULTURE2,ORIGINAL_BUFFER_SIZE_CULTURE3,ORIGINAL_BUFFER_SIZE_MILITARY,ORIGINAL_BUFFER_SIZE_SUPPORT };
    int current_buf_sizes[] = { CURRENT_BUFFER_SIZE_INDUSTRY,CURRENT_BUFFER_SIZE_CULTURE1,CURRENT_BUFFER_SIZE_CULTURE2,CURRENT_BUFFER_SIZE_CULTURE3, CURRENT_BUFFER_SIZE_MILITARY, CURRENT_BUFFER_SIZE_SUPPORT };
    buffer *buf_pointers[] = { industry,culture1,culture2,culture3,military,support };
    int buffer_count = 6;

    if (includes_buffer_size) {
        for (int i = 0; i < buffer_count; i++) {
            buf_sizes[i] = buffer_read_i32(buf_pointers[i]);
            buf_sizes[i] -= 4;
        }
    }

    // industry
    for (int i = 0; i < RESOURCE_MAX; i++) {
        data.industry[i].total = buffer_read_i32(industry);
    }
    for (int i = 0; i < RESOURCE_MAX; i++) {
        data.industry[i].active = buffer_read_i32(industry);
    }

    // culture 1
    data.buildings[BUILDING_THEATER].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_THEATER].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_AMPHITHEATER].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_AMPHITHEATER].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_COLOSSEUM].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_COLOSSEUM].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_HIPPODROME].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_HIPPODROME].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_SCHOOL].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_SCHOOL].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_LIBRARY].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LIBRARY].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_ACADEMY].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_ACADEMY].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_BARBER].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_BARBER].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_BATHHOUSE].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_BATHHOUSE].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_DOCTOR].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_DOCTOR].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_HOSPITAL].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_HOSPITAL].active = buffer_read_i32(culture1);
    data.buildings[BUILDING_SMALL_TEMPLE_CERES].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_SMALL_TEMPLE_MARS].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_SMALL_TEMPLE_VENUS].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LARGE_TEMPLE_CERES].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LARGE_TEMPLE_MARS].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_LARGE_TEMPLE_VENUS].total = buffer_read_i32(culture1);
    data.buildings[BUILDING_ORACLE].total = buffer_read_i32(culture1);

    // Arena counts upgraded culture ratings buildings

    if (buf_sizes[BUFFER_CULTURE_1] > ORIGINAL_BUFFER_SIZE_CULTURE1) {
        data.buildings[BUILDING_ARENA].total = buffer_read_i32(culture1);
        data.buildings[BUILDING_ARENA].active = buffer_read_i32(culture1);
        data.buildings[BUILDING_ARENA].upgraded = buffer_read_i32(culture1);
        data.buildings[BUILDING_THEATER].upgraded = buffer_read_i32(culture1);
        data.buildings[BUILDING_SCHOOL].upgraded = buffer_read_i32(culture1);
        data.buildings[BUILDING_LIBRARY].upgraded = buffer_read_i32(culture1);
        data.buildings[BUILDING_ACADEMY].upgraded = buffer_read_i32(culture1);
    }


    // culture 2
    data.buildings[BUILDING_ACTOR_COLONY].total = buffer_read_i32(culture2);
    data.buildings[BUILDING_ACTOR_COLONY].active = buffer_read_i32(culture2);
    data.buildings[BUILDING_GLADIATOR_SCHOOL].total = buffer_read_i32(culture2);
    data.buildings[BUILDING_GLADIATOR_SCHOOL].active = buffer_read_i32(culture2);
    data.buildings[BUILDING_LION_HOUSE].total = buffer_read_i32(culture2);
    data.buildings[BUILDING_LION_HOUSE].active = buffer_read_i32(culture2);
    data.buildings[BUILDING_CHARIOT_MAKER].total = buffer_read_i32(culture2);
    data.buildings[BUILDING_CHARIOT_MAKER].active = buffer_read_i32(culture2);

    // culture 3
    data.buildings[BUILDING_SMALL_TEMPLE_CERES].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_SMALL_TEMPLE_MARS].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_SMALL_TEMPLE_VENUS].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_LARGE_TEMPLE_CERES].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_LARGE_TEMPLE_MARS].active = buffer_read_i32(culture3);
    data.buildings[BUILDING_LARGE_TEMPLE_VENUS].active = buffer_read_i32(culture3);

    // military
    data.buildings[BUILDING_MILITARY_ACADEMY].total = buffer_read_i32(military);
    data.buildings[BUILDING_MILITARY_ACADEMY].active = buffer_read_i32(military);
    data.buildings[BUILDING_BARRACKS].total = buffer_read_i32(military);
    data.buildings[BUILDING_BARRACKS].active = buffer_read_i32(military);

    // support
    data.buildings[BUILDING_MARKET].total = buffer_read_i32(support);
    data.buildings[BUILDING_MARKET].active = buffer_read_i32(support);
    data.buildings[BUILDING_RESERVOIR].total = buffer_read_i32(support);
    data.buildings[BUILDING_RESERVOIR].active = buffer_read_i32(support);
    data.buildings[BUILDING_FOUNTAIN].total = buffer_read_i32(support);
    data.buildings[BUILDING_FOUNTAIN].active = buffer_read_i32(support);


    for (int i = 0; i < buffer_count; i++) {
        if (buf_sizes[i] > current_buf_sizes[i]) {
            buffer_skip(buf_pointers[i], buf_sizes[i] - current_buf_sizes[i]);
        }
    }

}
