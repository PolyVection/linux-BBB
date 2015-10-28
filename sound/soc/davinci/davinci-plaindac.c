/*
 * ASoC driver for PolyVection's PlainDAC+ / PlainDSP
 *
 * Author: Howard Mitchell <hm@hmdedded.co.uk>
 * Modified by: Philip Voigt <pv@polyvection.com> to work with PlainDAC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/of_platform.h>

#include <sound/soc.h>

static const struct of_device_id davinci_plaindac_dt_ids[] = {
	{ .compatible = "polyvection,am335x-plaindac", },
	{ }
};
MODULE_DEVICE_TABLE(of, davinci_plaindac_dt_ids);

static struct snd_soc_dai_link plaindac_dai = {
	.name			= "PlainDAC",
	.stream_name	= "PlainDAC HiFi",
	.codec_dai_name	= "pcm512x-hifi",
	.dai_fmt		= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_NB_IF,
};

static struct snd_soc_card plaindac_soc_card = {
	.owner = THIS_MODULE,
	.dai_link = &plaindac_dai,
	.num_links = 1,
};

static int davinci_plaindac_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *refclk;
	int ret = 0;

	plaindac_dai.codec_of_node = of_parse_phandle(np, "polyvection,audio-codec", 0);
	if (!plaindac_dai.codec_of_node)
		return -EINVAL;

	plaindac_dai.cpu_of_node = of_parse_phandle(np,"polyvection,mcasp-controller", 0);
	if (!plaindac_dai.cpu_of_node)
		return -EINVAL;

	plaindac_dai.platform_of_node = plaindac_dai.cpu_of_node;

	plaindac_soc_card.dev = &pdev->dev;
	ret = snd_soc_of_parse_card_name(&plaindac_soc_card, "polyvection,model");
	if (ret)
		return ret;

	ret = snd_soc_register_card(&plaindac_soc_card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n", ret);
	}
	return ret;
}

static int davinci_plaindac_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);

	snd_soc_unregister_card(card);

	return 0;
}

static struct platform_driver davinci_plaindac_driver = {
	.probe		= davinci_plaindac_probe,
	.remove		= davinci_plaindac_remove,
	.driver		= {
		.name	= "davinci_plaindac",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(davinci_plaindac_dt_ids),
	},
};

static int __init plaindac_init(void)
{
	/*
	 * When using device tree the devices will be created dynamically.
	 * Only register platfrom driver structure.
	 */
	return platform_driver_register(&davinci_plaindac_driver);
}

static void __exit plaindac_exit(void)
{
	platform_driver_unregister(&davinci_plaindac_driver);
	return;
}

module_init(plaindac_init);
module_exit(plaindac_exit);

MODULE_AUTHOR("Howard Mitchell");
MODULE_DESCRIPTION("plaindac ASoC driver");
MODULE_LICENSE("GPL");
