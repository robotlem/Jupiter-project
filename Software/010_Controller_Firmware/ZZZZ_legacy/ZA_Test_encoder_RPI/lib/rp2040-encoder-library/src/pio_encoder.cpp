/*
 * Copyright (C) 2023 Giovanni di Dio Bruno
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pio_encoder.h"

uint PioEncoder::offset_pio0;
bool PioEncoder::not_first_instance_pio0;
uint PioEncoder::offset_pio1;
bool PioEncoder::not_first_instance_pio1;


PioEncoder::PioEncoder(uint8_t _pin, PIO _pio, uint _sm, int _max_step_rate, bool wflip){
    static uint offset;
    pin=_pin;
    pio = _pio;
    sm = _sm;
    max_step_rate = _max_step_rate;
    flip(wflip);
}

void PioEncoder::begin(){
    bool &not_first = (pio == pio0) ? not_first_instance_pio0 : not_first_instance_pio1;
    uint &offset = (pio == pio0) ? offset_pio0 : offset_pio1;

    if (!not_first){
        offset = pio_add_program(pio, &quadrature_encoder_program);
    }
    not_first=true;
    if (sm==-1){
        sm = pio_claim_unused_sm(pio, true);
    }
    sm=sm;
    quadrature_encoder_program_init(pio,sm,offset,pin,max_step_rate);
}

void PioEncoder::reset(){
    quadrature_encoder_reset(pio, sm);
}

void PioEncoder::flip(const bool x){
    if (x){
        flip_it=-1;
    }
    else{
        flip_it=1;
    }
}

int PioEncoder::getCount(){
    return flip_it*quadrature_encoder_get_count(pio, sm);
}