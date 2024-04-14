/**
  ******************************************************************************
  * @file    sine_model_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Apr 12 21:18:37 2024
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "sine_model_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_sine_model_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_sine_model_weights_array_u64[161] = {
  0x3ea696583ea06f15U, 0x3e2cf74c3e71f411U, 0x3e8e6976bd2d4d30U, 0x3eb3b6bc3ec48429U,
  0xbee43ef8bd87a0e8U, 0xbe7e6d203f0dcedbU, 0xbf162e5e3e9d75bfU, 0xbd2bff303b113536U,
  0xbf20ad96becb86d2U, 0x3f0edd753e57629aU, 0x3e56546500000000U, 0xbed7e98ebf00812bU,
  0x0U, 0x3cff3104U, 0x3eed3c61U, 0xbcaed6a2U,
  0x3dd4418a3e56fe6dU, 0xbe81be2e3dc082e5U, 0xbda0424a3e471efeU, 0x3e558309be6cde41U,
  0xbe18bff83e60b926U, 0x3da2275c3ea221ddU, 0xbebfdc243e8724e6U, 0xbecf692bbcb6cd13U,
  0xbea0b5ccbe842168U, 0x3edfd7473ee26aabU, 0x3d2c0f033d6a4b10U, 0xbedec836be8355b9U,
  0x3e2a11563ed9baadU, 0x3e93c14bbe85a440U, 0x3d5489703e47fdfcU, 0xbdfa48f03e023ebdU,
  0x3e2da3f6be7b8235U, 0x3deae1e93e83e0f1U, 0x3d3cf4023afacc00U, 0x3e5a9348bd566d65U,
  0xbed56f77beda030fU, 0x3e98c4313e2b980dU, 0xbcf5c4c03eaaed0fU, 0xbed387fcbeb8f53cU,
  0x3eb8e2023e01fe2dU, 0x3db94a4cbe3920b2U, 0x3ce877a23e8f72b5U, 0x3ed7d4e63ee343d7U,
  0x3e5c064abec6e997U, 0x3e359212bea727c0U, 0x3ed13ba9be64775bU, 0x3d5fc9883e73afd3U,
  0xbd5d51f83dd764ccU, 0xbeb98ce2be0f47c6U, 0x3b32bd80be872872U, 0xbe8e4dc03ea7cfb5U,
  0xbed41ef53ec891afU, 0xbc6bdd80be2cb532U, 0xbd1847e83e54327eU, 0x3dd3d3b43e4a1886U,
  0xbf1d663ebf07ca45U, 0x3e91bc113ec94091U, 0x3db4181abe1e30eaU, 0xbdd8a0cebd253c5bU,
  0x3c9043e0be09a160U, 0xbe94ef753e098b26U, 0x3e0ff546bea83754U, 0x3eb8c84f3e5d546cU,
  0xbe67b2b9bf007f06U, 0xbdbe5d713eec2f1eU, 0x3d6fb5453db0c494U, 0xbeed9b16bf089cefU,
  0xbe93ac83be718d68U, 0xbe355a9ebf26f786U, 0xbd6555d0be900a5cU, 0xbe0e0d703cebac80U,
  0x3f02be833d901f53U, 0xbec029b33e558c96U, 0xbe2440363da7dcc4U, 0x3e5f45823e2dd0b7U,
  0xbd8e32bc3edc8e6fU, 0x3eb4b6f53e183c40U, 0xbec23214be684a35U, 0x3d3894803ec3fba4U,
  0xbebded74bebd035bU, 0xbe9407d238ab4000U, 0xbe9f0f68beada665U, 0x3e965ccfbd151740U,
  0x3dae550cbd677c48U, 0xbe1d4ce8be3ca0edU, 0x3eb1f2233e9649bfU, 0x3e7e0a4e3ed9654bU,
  0xbf19cfb2be137617U, 0x3ef5825abe2dc96fU, 0xbedb34b33e9d8b47U, 0xbe37c810bdcdd3c5U,
  0x3dee89acbe1b6bbaU, 0x3e9c3a9fbf2c0937U, 0xbe655e783ebbd21dU, 0xbe10435a3dffbf24U,
  0x3e01d01abc40447fU, 0xbeaac71c3e79e6daU, 0x3ef1cbdd3c5b6680U, 0x3dd61da0be77a565U,
  0xbea8318d3dceeebcU, 0x3c0ab4603eea2212U, 0x3ddb4d2cbe4e6539U, 0xbe8106083ec25d23U,
  0xbed562bbbed071f5U, 0x3f09de9e3d4c07c8U, 0xbd692589bd72b5d8U, 0xbeb096bfbe3a4f8aU,
  0x3e6f6cf23ebe520fU, 0x3d064530bedefddaU, 0x3dcaa8c43e88f8cbU, 0xbecc84d6bea56402U,
  0x3d1ef600be3bc27eU, 0xbe809e62be9b9f1aU, 0x3e9d69adbe1c7dc4U, 0xbe0e1f383e0c9aeaU,
  0xbe1ec2c23e8d5487U, 0x3e97a979be710208U, 0x3eba92bdbd22f390U, 0x3ed20cfb3e85b671U,
  0xbe9bb64d3eb0fd36U, 0x3ead83be3ddf6462U, 0x3ce5d26bbe069ee2U, 0x3d9beaeabe0edddcU,
  0xbeb8346c3ec47eb5U, 0xbd0ff0a03eeef82dU, 0xbeb9a475be853826U, 0xbdb5bab8beb012fdU,
  0x3ee094523ec2c898U, 0xbeba91283e62c00cU, 0x3c9659eb3e04a272U, 0x3d3b16b83ed33beeU,
  0xbddd9c74bdbf692cU, 0x3e4ba1823ea50becU, 0xbdad18ac3caaef87U, 0x3e85d785be505ca8U,
  0xbf0e4ed8bf18bee6U, 0x3ed0be523ee5a6a6U, 0x3cb6f706bc825910U, 0xbed25e9ebea05ab1U,
  0xbea44082bebacbb0U, 0x3e9e56a93db0247dU, 0x3b37f6803e191593U, 0xbecd18583ea08b0bU,
  0x3e854127bdca35a6U, 0xbe9d79b7bd94ab3bU, 0x3e7f2fa300000000U, 0xbf1101d63eb8567dU,
  0x3eb4904c00000000U, 0x3ea5e531bd50c305U, 0xbcefb4df00000000U, 0x3e89614cbccc5528U,
  0x3f4d69dc3e0e2bc0U, 0x3fa422b5bed7353eU, 0x3f8889ed3ef56cc0U, 0x3ffdc6adbf79e6bbU,
  0xbf5e6b12bef5b05cU, 0xbf449cb1bf16f47eU, 0xbead6f84becf5138U, 0x3f8113f23ece1f41U,
  0xbdf3b70bU,
};


ai_handle g_sine_model_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_sine_model_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

