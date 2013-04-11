/* 
 * material {
 * 	pass : [ 'opaque', 'transparent' ] , -- default pass tags for all qualities
 * 	quality : {
 * 		require : {
 * 			global : 'medium',	-- global quality setting'
 * 		},
 * 		pass : {
 * 			tag : [],
 * 			texture_unit : [
 * 			{},{}, ..
 * 			],
 * 			input : {"position", "color", "normal", "custom1" },
 * 			shader : {
 * 				glsl : "",
 * 				cg : "",
 * 				hlsl : ""
 * 			}
 *
 * 		}
 * 	}
 *
 * } 
 */

class MaterialResource : public Resource {

};
