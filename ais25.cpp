// Msg 25 - I - Single slot binary message

#include "ais.h"

Ais25::Ais25(const char *nmea_payload, const size_t pad) {
    assert(nmea_payload);
    assert(pad < 6);
    init();

    const size_t num_bits = strlen(nmea_payload) * 6 - pad;
    if (num_bits < 40 || num_bits > 168) { status = AIS_ERR_BAD_BIT_COUNT; return; }

    std::bitset<168> bs;
    status = aivdm_to_bits(bs, nmea_payload);
    if (had_error()) return;

    message_id = ubits(bs, 0, 6);
    if (25 != message_id) {status = AIS_ERR_WRONG_MSG_TYPE; return;}
    repeat_indicator = ubits(bs, 6, 2);
    mmsi = ubits(bs, 8, 30);

    const bool addressed = bs[38];
    use_app_id = bs[39];
    if (addressed) {
      dest_mmsi_valid = true;
      dest_mmsi = ubits(bs, 40, 30);
      if (use_app_id) {
        dac = ubits(bs, 70, 10);
        fi = ubits(bs, 80, 6);
      }
      // TODO: deal with payload
    } else {
      // broadcast
      if (use_app_id) {
        dac = ubits(bs, 40, 10);
        fi = ubits(bs, 50, 6);
      }
      // TODO: deal with payload - probably need to pass in the spare bits
    }
}
